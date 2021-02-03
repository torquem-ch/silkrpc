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

#include "transaction_database.hpp"

#include <climits>
#include <exception>

#include <silkrpc/common/log.hpp>
#include <silkrpc/common/util.hpp>

namespace silkrpc::ethdb::kv {

asio::awaitable<bool> TransactionDatabase::has(const std::string& table_name, const silkworm::Bytes& key) {
    co_return false;
}

asio::awaitable<silkworm::Bytes> TransactionDatabase::get(const std::string& table, const silkworm::Bytes& key) {
    const auto cursor = co_await tx_.cursor(table);
    //SILKRPC_INFO << "cursor_id: " << cursor->cursor_id() << "\n" << std::flush;
    const auto kv_pair = co_await cursor->seek(key);
    co_return kv_pair.value;
}

asio::awaitable<void> TransactionDatabase::walk(const std::string& table, const silkworm::Bytes& start_key, uint32_t fixed_bits, core::rawdb::Walker w) {
    using namespace silkworm;
    const auto fixed_bytes = (fixed_bits + 7) / CHAR_BIT;
    //SILKRPC_INFO << "fixed_bits: " << fixed_bits << " fixed_bytes: " << fixed_bytes << "\n" << std::flush;
    const auto shift_bits = fixed_bits & 7;
    uint8_t mask{0xff};
    if (shift_bits != 0) {
        mask = 0xff << (CHAR_BIT - shift_bits);
    }
    //SILKRPC_INFO << "mask: " << mask << "\n" << std::flush;

    const auto cursor = co_await tx_.cursor(table);
    //SILKRPC_INFO << "cursor_id: " << cursor->cursor_id() << "\n" << std::flush;
    auto kv_pair = co_await cursor->seek(start_key);
    auto k = kv_pair.key;
    auto v = kv_pair.value;
    //SILKRPC_INFO << "k: " << k << " v: " << v << "\n" << std::flush;
    //SILKRPC_INFO << "k.empty(): " << k.empty() << "\n" << std::flush;
    //SILKRPC_INFO << "k.size(): " << k.size() << "\n" << std::flush;
    //SILKRPC_INFO << "k.compare(0, fixed_bytes-1, start_key, 0, fixed_bytes-1): " << k.compare(0, fixed_bytes-1, start_key, 0, fixed_bytes-1) << "\n" << std::flush;
    //SILKRPC_INFO << "k[fixed_bytes-1]: " << k[fixed_bytes-1] << "\n" << std::flush;
    while (
        !k.empty() &&
        k.size() >= fixed_bytes &&
        (fixed_bits == 0 || k.compare(0, fixed_bytes-1, start_key, 0, fixed_bytes-1) == 0 && (k[fixed_bytes-1]&mask) == (start_key[fixed_bytes-1]&mask))
    ) {
        const auto go_on = w(k, v);
        if (!go_on) {
            break;
        }
        kv_pair = co_await cursor->next();
        k = kv_pair.key;
        v = kv_pair.value;
    }

    co_return;
}

void TransactionDatabase::close() {}

} // namespace silkrpc::ethdb::kv