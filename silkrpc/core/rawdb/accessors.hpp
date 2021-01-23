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

#ifndef SILKRPC_CORE_RAWDB_ACCESSORS_H_
#define SILKRPC_CORE_RAWDB_ACCESSORS_H_

#include <silkrpc/config.hpp>

#include <memory>
#include <string>

#include <asio/awaitable.hpp>

#include <silkworm/common/util.hpp>

namespace silkrpc::core::rawdb {

typedef std::function<bool(silkworm::Bytes&,silkworm::Bytes&)> Walker;

class DatabaseReader {
public:
    virtual asio::awaitable<bool> has(const std::string& table, const silkworm::Bytes& key) = 0;

    virtual asio::awaitable<silkworm::Bytes> get(const std::string& table, const silkworm::Bytes& key) = 0;

    virtual asio::awaitable<void> walk(const std::string& table, const silkworm::Bytes& start_key, uint32_t fixed_bits, Walker w) = 0;
};

} // namespace silkrpc::core::rawdb

#endif  // SILKRPC_CORE_RAWDB_ACCESSORS_H_
