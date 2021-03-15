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

#include "stages.hpp"

#include <exception>

#include <boost/endian/conversion.hpp>

#include <silkworm/db/tables.hpp>

#include <silkrpc/common/log.hpp>

namespace silkrpc::stages {

class Exception : public std::exception {
 public:
    explicit Exception(const char* message) : message_{message} {};
    explicit Exception(const std::string& message) : message_{message} {};
    virtual ~Exception() noexcept {};
    const char* what() const noexcept override { return message_.c_str(); }

 protected:
    std::string message_;
};

asio::awaitable<uint64_t> get_sync_stage_progress(const core::rawdb::DatabaseReader& db_reader, const Bytes& stage_key) {
    using namespace silkworm; // NOLINT(build/namespaces)
    SILKRPC_TRACE << "silkrpc::stages::get_sync_stage_progress stage_key: " << stage_key << "\n";
    const auto value = co_await db_reader.get("SSP2", stage_key);
    SILKRPC_TRACE << "silkrpc::stages::get_sync_stage_progress value: " << value << "\n";
    if (value.length() == 0) {
        co_return 0;
    }
    if (value.length() < 8) {
        throw Exception("data too short, expected 8 got " + std::to_string(value.length()));
    }
    uint64_t block_height = boost::endian::load_big_u64(value.substr(0, 8).data());
    SILKRPC_TRACE << "silkrpc::stages::get_sync_stage_progress block_height: " << block_height << "\n";
    co_return block_height;
}

} // namespace silkrpc::stages
