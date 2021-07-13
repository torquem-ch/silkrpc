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

#ifndef SILKRPC_CORE_GAS_PRICE_ORACLE_HPP_
#define SILKRPC_CORE_GAS_PRICE_ORACLE_HPP_

#include <string>

#include <silkrpc/config.hpp> // NOLINT(build/include_order)

#include <asio/awaitable.hpp>
#include <asio/thread_pool.hpp>
#include <silkworm/chain/config.hpp>
#include <silkworm/common/util.hpp>
#include <silkworm/state/buffer.hpp>
#include <silkworm/types/block.hpp>
#include <silkworm/types/transaction.hpp>

#include <silkrpc/context_pool.hpp>
#include <silkrpc/core/remote_buffer.hpp>
#include <silkrpc/core/rawdb/accessors.hpp>

namespace silkrpc {

const intx::uint256 wei = 1;
const intx::uint256 g_wei = 1E9;

const intx::uint256 default_max_price = 500 * g_wei;
const intx::uint256 default_min_price = 2 * wei;

const std::uint8_t check_blocks = 20;
const std::uint8_t samples = 3;
const std::uint8_t percentile = 60;

typedef struct GasPriceOracleConfig {
    int check_blocks;
    int percentile;
    intx::uint256 default_price;
    intx::uint256 max_price;
    intx::uint256 ignore_price;
} GasPriceOracleConfig;

static const GasPriceOracleConfig GPO_FULL_CONFIG = {check_blocks, percentile, 0, default_max_price, default_min_price};

class GasPriceOracle {
public:
    explicit GasPriceOracle(const core::rawdb::DatabaseReader& db_reader, const GasPriceOracleConfig &config) 
        : db_reader_(db_reader), config_(config) {}
    virtual ~GasPriceOracle() {}

    GasPriceOracle(const GasPriceOracle&) = delete;
    GasPriceOracle& operator=(const GasPriceOracle&) = delete;

    asio::awaitable<intx::uint256> suggested_price();

private:
    asio::awaitable<void> load_block_prices(uint64_t block_number, uint64_t limit, std::vector<intx::uint256>& tx_prices);

    const core::rawdb::DatabaseReader& db_reader_;
    const GasPriceOracleConfig& config_;
};

} // namespace silkrpc

#endif  // SILKRPC_CORE_GAS_PRICE_ORACLE_HPP_
