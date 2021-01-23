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

#ifndef SILKRPC_ETHDB_TRANSACTIONDATABASE_H_
#define SILKRPC_ETHDB_TRANSACTIONDATABASE_H_

#include "transaction.hpp"

#include <string>

#include <silkworm/common/util.hpp>
#include <silkrpc/core/rawdb/accessors.hpp>

namespace silkrpc::ethdb::kv {

class TransactionDatabase : public core::rawdb::DatabaseReader {
public:
    TransactionDatabase(kv::Transaction& tx) : tx_(tx) {}

    TransactionDatabase(const TransactionDatabase&) = delete;
    TransactionDatabase& operator=(const TransactionDatabase&) = delete;

    asio::awaitable<bool> has(const std::string& table, const silkworm::Bytes& key) override;

    asio::awaitable<silkworm::Bytes> get(const std::string& table, const silkworm::Bytes& key) override;

    asio::awaitable<void> walk(const std::string& table, const silkworm::Bytes& start_key, uint32_t fixed_bits, core::rawdb::Walker w) override;

    void close();
private:
    kv::Transaction& tx_;
};

} // namespace silkrpc::ethdb::kv

#endif  // SILKRPC_ETHDB_TRANSACTIONDATABASE_H_
