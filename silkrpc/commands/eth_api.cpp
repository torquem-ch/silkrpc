/*
    Copyright 2020 The Silkrpc Authors

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "eth_api.hpp"

#include <algorithm>
#include <exception>
#include <iostream>

#include <silkworm/core/silkworm/common/util.hpp>
#include <silkworm/core/silkworm/types/receipt.hpp>
#include <silkworm/db/silkworm/db/tables.hpp>
#include <silkrpc/common/log.hpp>
#include <silkrpc/core/blocks.hpp>
#include <silkrpc/core/rawdb/chain.hpp>
#include <silkrpc/ethdb/bitmap/database.hpp>
#include <silkrpc/json/types.hpp>

namespace silkrpc::commands {

// https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_blockNumber
asio::awaitable<void> EthereumRpcApi::handle_eth_block_number(const nlohmann::json& request, nlohmann::json& reply) {
    auto tx = database_->begin();
    ethdb::kv::TransactionDatabase tx_database{*tx};

    try {
        const auto block_height = co_await core::get_current_block_number(tx_database);
        reply = json::make_json_content(request["id"], block_height);
    } catch (const std::exception& e) {
        SILKRPC_ERROR << "exception: " << e.what() << "\n";
        reply = json::make_json_error(request["id"], e.what());

        tx->rollback(); // co_await when implemented
    }

    co_return;
}

// https://github.com/ethereum/wiki/wiki/JSON-RPC#eth_getLogs
asio::awaitable<void> EthereumRpcApi::handle_eth_get_logs(const nlohmann::json& request, nlohmann::json& reply) {
    auto filter = request["params"].get<json::Filter>();
    std::cout << "filter=" << filter << "\n" << std::flush;

    std::vector<silkworm::Log> logs;

    auto tx = database_->begin();
    ethdb::kv::TransactionDatabase tx_database{*tx};

    try {
        uint64_t start{}, end{};
        if (filter.block_hash.has_value()) {
            auto block_hash = silkworm::to_bytes32(silkworm::from_hex(filter.block_hash.value()));
            auto block_number = co_await core::rawdb::read_header_number(tx_database, block_hash);
            start = end = block_number;
        } else {
            auto latest_block_number = co_await core::get_latest_block_number(tx_database);
            start = filter.from_block.value_or(latest_block_number);
            end = filter.from_block.value_or(latest_block_number);
        }

        Roaring block_numbers;
        block_numbers.addRange(start, end + 1);

        if (filter.topics.has_value()) {
            auto topics_bitmap = co_await get_topics_bitmap(tx_database, filter.topics.value(), start, end);
            if (!topics_bitmap.isEmpty()) {
                if (block_numbers.isEmpty()) {
                    block_numbers = topics_bitmap;
                } else {
                    block_numbers &= topics_bitmap;
                }
            }
        }

        if (filter.addresses.has_value()) {
            auto addresses_bitmap = co_await get_addresses_bitmap(tx_database, filter.addresses.value(), start, end); // [2] implement
            if (!addresses_bitmap.isEmpty()) {
                if (block_numbers.isEmpty()) {
                    block_numbers = addresses_bitmap;
                } else {
                    block_numbers &= addresses_bitmap;
                }
            }
        }

        if (block_numbers.cardinality() == 0) {
            reply = json::make_json_content(request["id"], logs);
            co_return;
        }

        std::vector<uint32_t> ans{uint32_t(block_numbers.cardinality())};
        block_numbers.toUint32Array(ans.data());
        for (auto block_to_match : block_numbers) {
            auto block_hash = co_await core::rawdb::read_canonical_block_hash(tx_database, uint64_t(block_to_match));
            if (block_hash == evmc::bytes32{}) {
                reply = json::make_json_content(request["id"], logs);
                co_return;
            }

            auto receipts = co_await get_receipts(tx_database, uint64_t(block_to_match), block_hash);
            std::vector<silkworm::Log> unfiltered_logs{receipts.size()};
            for (auto receipt: receipts) {
                unfiltered_logs.insert(unfiltered_logs.end(), receipt.logs.begin(), receipt.logs.end());
            }
            auto filtered_logs = filter_logs(unfiltered_logs, filter);
            logs.insert(logs.end(), filtered_logs.begin(), filtered_logs.end());
        }

        reply = json::make_json_content(request["id"], logs);
    } catch (const std::exception& e) {
        SILKRPC_ERROR << "exception: " << e.what() << "\n";
        reply = json::make_json_error(request["id"], e.what());

        tx->rollback(); // co_await when implemented
    }

    co_return;
}

asio::awaitable<Roaring> EthereumRpcApi::get_topics_bitmap(core::rawdb::DatabaseReader& db_reader, json::FilterTopics& topics, uint64_t start, uint64_t end) {
    Roaring result_bitmap;
    for (auto subtopics : topics) {
        Roaring subtopic_bitmap;
        for (auto topic : subtopics) {
            auto topic_key = silkworm::from_hex(topic);
            auto bitmap = co_await ethdb::bitmap::get(db_reader, silkworm::db::table::kLogTopicIndex.name, topic_key, start, end);
            subtopic_bitmap |= bitmap;
        }
        result_bitmap &= subtopic_bitmap;
    }
    co_return result_bitmap;
}

asio::awaitable<Roaring> EthereumRpcApi::get_addresses_bitmap(core::rawdb::DatabaseReader& db_reader, json::FilterAddresses& addresses, uint64_t start, uint64_t end) {
    Roaring result_bitmap;
    for (auto address : addresses) {
        auto address_key = silkworm::from_hex(address);
        auto bitmap = co_await ethdb::bitmap::get(db_reader, silkworm::db::table::kLogAddressIndex.name, address_key, start, end);
        result_bitmap |= bitmap;
    }
    co_return result_bitmap;
}

asio::awaitable<Receipts> EthereumRpcApi::get_receipts(core::rawdb::DatabaseReader& db_reader, uint64_t number, evmc::bytes32 hash) {
    // TODO: implement
    co_return Receipts{};
}

std::vector<silkworm::Log> EthereumRpcApi::filter_logs(std::vector<silkworm::Log>& logs, const json::Filter& filter) {
    std::vector<silkworm::Log> filtered_logs;

    auto addresses = filter.addresses;
    auto topics = filter.topics;
    for (auto log : logs) {
        auto log_address{silkworm::to_hex(log.address)};
        if (addresses.has_value() && std::find(addresses.value().begin(), addresses.value().end(), log_address) == addresses.value().end()) {
            continue;
        }
        auto matches = true;
        if (topics.has_value()) {
            if (topics.value().size() > log.topics.size()) {
                continue;
            }
            for (size_t i{0}; i < topics.value().size(); i++) {
                auto log_topic{silkworm::to_hex(log.topics[i])};
                auto subtopics = topics.value()[i];
                auto matches_subtopics = subtopics.empty(); // empty rule set == wildcard
                for (auto topic : subtopics) {
                    if (log_topic == topic) {
                        matches_subtopics = true;
                        break;
                    }
                }
                if (!matches_subtopics) {
                    matches = false;
                    break;
                }
            }
        }
        if (matches) {
            filtered_logs.push_back(log);
        }
    }
    return filtered_logs;
}

} // namespace silkrpc::commands