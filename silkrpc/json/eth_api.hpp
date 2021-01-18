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

#ifndef SILKRPC_JSON_ETH_API_H_
#define SILKRPC_JSON_ETH_API_H_

#include <silkrpc/config.hpp>

#include <memory>
#include <vector>

#include <asio/awaitable.hpp>
#include <evmc/evmc.hpp>
#include <nlohmann/json.hpp>

#include <silkworm/core/silkworm/types/receipt.hpp>
#include <silkrpc/croaring/roaring.hh>
#include <silkrpc/json/types.hpp>
#include <silkrpc/ethdb/kv/database.hpp>
#include <silkrpc/ethdb/kv/transaction.hpp>
#include <silkrpc/ethdb/kv/transaction_database.hpp>

namespace silkrpc::http { class RequestHandler; }

namespace silkrpc::json {

typedef std::vector<silkworm::Receipt> Receipts;

class EthereumRpcApi {
public:
    EthereumRpcApi(const EthereumRpcApi&) = delete;
    EthereumRpcApi& operator=(const EthereumRpcApi&) = delete;

    explicit EthereumRpcApi(std::unique_ptr<ethdb::kv::Database>& database) : database_(database) {}

    virtual ~EthereumRpcApi() {}

private:
    asio::awaitable<void> handle_eth_block_number(const nlohmann::json& request, nlohmann::json& reply);
    asio::awaitable<void> handle_eth_get_logs(const nlohmann::json& request, nlohmann::json& reply);

    Roaring get_topics_bitmap(ethdb::kv::TransactionDatabase& tx_db, eth::FilterTopics& topics, uint64_t start, uint64_t end);
    Roaring get_addresses_bitmap(ethdb::kv::TransactionDatabase& tx_db, eth::FilterAddresses& addresses, uint64_t start, uint64_t end);
    asio::awaitable<Receipts> get_receipts(ethdb::kv::TransactionDatabase& tx_db, uint64_t number, evmc::bytes32 hash);
    std::vector<silkworm::Log> filter_logs(std::vector<silkworm::Log>& unfiltered, const eth::Filter& filter);

    std::unique_ptr<ethdb::kv::Database>& database_;

    friend class silkrpc::http::RequestHandler;
};

} // namespace silkrpc::json

#endif  // SILKRPC_JSON_ETH_API_H_
