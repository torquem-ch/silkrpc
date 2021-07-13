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

#include "gas_price_oracle.hpp"

#include <utility>

#include <asio/compose.hpp>
#include <asio/post.hpp>
#include <asio/use_awaitable.hpp>
#include <silkrpc/core/blocks.hpp>
#include <silkrpc/core/rawdb/chain.hpp>

#include <silkrpc/common/log.hpp>

namespace silkrpc {

evmc::address get_coinbase(const silkworm::BlockHeader& block_header) {
    return block_header.beneficiary;
}

intx::uint256 get_block_base_fee(const silkworm::BlockHeader& block_header) {
    return 0;
}

intx::uint256 get_effective_gas_price(const silkworm::Transaction& transaction, intx::uint256 base_fee) {
    return transaction.gas_price;
}

struct priceLess {
    bool operator() (const intx::uint256& p1, const intx::uint256& p2) const { 
        return p1 < p2;
    } 
};

struct txPriceLess {
    txPriceLess(intx::uint256 base_fee) : base_fee_(base_fee) {}

    intx::uint256 base_fee_;
    bool operator() (const silkworm::Transaction* t1, const silkworm::Transaction* t2) const {
        // const auto egp1 = get_effective_gas_price(*t1, base_fee_);
        // const auto egp2 = get_effective_gas_price(*t2, base_fee_);
        // SILKRPC_LOG << "egp1 " << silkworm::to_hex(silkworm::rlp::big_endian(egp1)) << " egp2 " << silkworm::to_hex(silkworm::rlp::big_endian(egp2)) << "\n";
        return get_effective_gas_price(*t1, base_fee_) < get_effective_gas_price(*t2, base_fee_);
    } 
};

asio::awaitable<intx::uint256> GasPriceOracle::suggested_price() {
    SILKRPC_LOG << "GasPriceOracle::suggested_price checkBlock: " << unsigned(config_.check_blocks) 
        << " percentile: " << unsigned(config_.percentile)
        << " ignore_price: 0x" << silkworm::to_hex(silkworm::rlp::big_endian(config_.ignore_price)) 
        << " max_price: 0x" << silkworm::to_hex(silkworm::rlp::big_endian(config_.max_price)) << " start\n";

    auto block_number = co_await core::get_block_number(silkrpc::core::kLatestBlockId, db_reader_);
    block_number = 4000000;
    // TODO basta l'header
    // const auto block_with_hash = co_await core::rawdb::read_block_by_number(tx_database, block_number);
    // auto block_number = block_with_hash.block.header.number;
    SILKRPC_LOG << "block_number " << block_number << "\n";

    const auto max_size = config_.check_blocks * silkrpc::samples;
    SILKRPC_LOG << "max_size " << max_size << "\n";

    std::vector<intx::uint256> tx_prices;
    std::uint8_t count{0};
    SILKRPC_LOG << "GasPriceOracle::suggested_price starting prices vector size: " << tx_prices.size() << "\n";
    while (tx_prices.size() < max_size && block_number > 0) {
        SILKRPC_DEBUG << "block_number " << block_number << "\n";
        co_await load_block_prices(block_number--, silkrpc::samples, tx_prices);
        if (++count > max_size)
            break;
    }
    SILKRPC_LOG << "GasPriceOracle::suggested_price final prices vector size: " << tx_prices.size() << "\n";

    std::sort(tx_prices.begin(), tx_prices.end(), priceLess());

    intx::uint256 price = config_.default_price;
    if (tx_prices.size() > 0) {
        auto position = (tx_prices.size() - 1) * config_.percentile / 100; // 60 is silkrpc::percentile
        SILKRPC_LOG << "GasPriceOracle::suggested_price getting price in position: " << position << "\n";
        if (tx_prices.size() > position) {
            price = tx_prices[position];
        }
    }
    SILKRPC_LOG << "GasPriceOracle::suggested_price selected price form transactions: 0x" << silkworm::to_hex(silkworm::rlp::big_endian(price)) << "\n";

    if (price > silkrpc::default_max_price) {
        price = silkrpc::default_max_price;
    }

    SILKRPC_LOG << "GasPriceOracle::suggested_price final price: 0x" << silkworm::to_hex(silkworm::rlp::big_endian(price)) << "\n";
    co_return price;
}

asio::awaitable<void> GasPriceOracle::load_block_prices(uint64_t block_number, uint64_t limit, std::vector<intx::uint256>& tx_prices) {
    SILKRPC_LOG << "GasPriceOracle::load_block_prices processing block: " << block_number << "\n";

    const auto block_with_hash = co_await core::rawdb::read_block_by_number(db_reader_, block_number);
    const auto base_fee = get_block_base_fee(block_with_hash.block.header);
    const auto coinbase = get_coinbase(block_with_hash.block.header);

    SILKRPC_LOG << "GasPriceOracle::load_block_prices # transactions in block: " << block_with_hash.block.transactions.size() << "\n";
    SILKRPC_LOG << "GasPriceOracle::load_block_prices # block beneficiary: 0x" << coinbase << "\n";

    std::vector<intx::uint256> block_prices;
    block_prices.reserve(block_with_hash.block.transactions.size());
    for (auto itr = block_with_hash.block.transactions.begin(); itr < block_with_hash.block.transactions.end(); itr++) {
        auto transaction = *itr;
        const auto effective_gas_price = get_effective_gas_price(transaction, base_fee);
        if (effective_gas_price < config_.ignore_price) {
            continue;
        }
        const auto sender = transaction.from;
        if (sender == coinbase) {
            continue;
        }
        block_prices.push_back(effective_gas_price);

        const auto ethash_hash = hash_of_transaction(transaction);
        silkworm::ByteView hash_view{ethash_hash.bytes, silkworm::kHashLength};
        SILKRPC_LOG << "pushing transaction with hash: 0x" << silkworm::to_bytes32(hash_view) << " : get_effective_gas_price 0x" << silkworm::to_hex(silkworm::rlp::big_endian(effective_gas_price)) << "\n";
    }
    SILKRPC_LOG << "GasPriceOracle::load_block_prices # loaded prices: " << block_prices.size() << "\n";

    // std::make_heap(transactions.begin(), transactions.end(), txPriceLess(base_fee));
    std::sort(block_prices.begin(), block_prices.end(), priceLess());
    for (int count = 0; count < block_prices.size(); count++) {
        SILKRPC_LOG << "AFTER SORT idx " << count << " : get_effective_gas_price 0x" << silkworm::to_hex(silkworm::rlp::big_endian(block_prices[count])) << "\n";
    }

    for (auto itr = block_prices.begin(); itr < block_prices.end(); itr++) {
        const auto effective_gas_price = *itr;
        SILKRPC_LOG << "pushing effective_gas_price 0x" << silkworm::to_hex(silkworm::rlp::big_endian(effective_gas_price)) << "\n";

        tx_prices.push_back(effective_gas_price);
        if (tx_prices.size() >= limit) {
            break;
        }
    }
    SILKRPC_LOG << "tx_prices size " << tx_prices.size() << "\n";
	SILKRPC_LOG << "**************************************************\n";
}

// asio::awaitable<void> GasPriceOracle::load_block_prices(uint64_t block_number, uint64_t limit, std::vector<intx::uint256>& tx_prices) {
//     SILKRPC_LOG << "GasPriceOracle::load_block_prices processing block: " << block_number << "\n";

//     const auto block_with_hash = co_await core::rawdb::read_block_by_number(db_reader_, block_number);
//     const auto base_fee = get_block_base_fee(block_with_hash.block.header);

//     std::vector<silkworm::Transaction *> transactions;
//     for (auto itr = block_with_hash.block.transactions.begin(); itr < block_with_hash.block.transactions.end(); itr++) {
//         auto transaction = *itr;
//         auto address = std::addressof(*itr);
//         const auto price = get_effective_gas_price(transaction, base_fee);
//         const auto ethash_hash = hash_of_transaction(transaction);
//         silkworm::ByteView hash_view{ethash_hash.bytes, silkworm::kHashLength};
//         SILKRPC_LOG << "transaction address: 0x" << address << "\n";
//         SILKRPC_LOG << "pushing transaction with hash: 0x" << silkworm::to_bytes32(hash_view) << " : get_effective_gas_price " << silkworm::to_hex(silkworm::rlp::big_endian(price)) << "\n";
//         transactions.push_back(&transaction);
//     }
//     SILKRPC_LOG << "GasPriceOracle::load_block_prices # transactions in block: " << transactions.size() << "\n";

//     for (auto itr = transactions.begin(); itr < transactions.end(); itr++) {
//         auto transaction = *itr;
//         const auto price = get_effective_gas_price(*transaction, base_fee);
//         const auto ethash_hash = hash_of_transaction(*transaction);
//         silkworm::ByteView hash_view{ethash_hash.bytes, silkworm::kHashLength};
//         SILKRPC_LOG << "transaction with hash: 0x" << silkworm::to_bytes32(hash_view) << " : get_effective_gas_price " << silkworm::to_hex(silkworm::rlp::big_endian(price)) << "\n";
//     }

//     // std::make_heap(transactions.begin(), transactions.end(), txPriceLess(base_fee));
//     std::sort(transactions.begin(), transactions.end(), txPriceLess(base_fee));
//     for (int count = 0; count < transactions.size(); count++) {
//         const auto price = get_effective_gas_price(*transactions[count], base_fee);
//         const auto ethash_hash = hash_of_transaction(*transactions[count]);
//         silkworm::ByteView hash_view{ethash_hash.bytes, silkworm::kHashLength};
//         SILKRPC_LOG << "idx " << count << ") hash: 0x" << silkworm::to_bytes32(hash_view) << " : get_effective_gas_price " << silkworm::to_hex(silkworm::rlp::big_endian(price)) << "\n";
//     }

//     while (transactions.size() > 0) {
//         std::pop_heap(transactions.begin(), transactions.end());
//         const auto transaction = transactions.front();
//         transactions.pop_back();

//         const auto effective_gas_price = get_effective_gas_price(*transaction, base_fee);
//         if (effective_gas_price < config_.ignore_price) {
//             continue;
//         }
//         const auto sender = transaction->from;
//         // TODO se il sender == block miner allora continue

//         tx_prices.push_back(effective_gas_price);
//         if (tx_prices.size() >= limit) {
//             break;
//         }
//     }
// }

} // namespace silkrpc
