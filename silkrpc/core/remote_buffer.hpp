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

#ifndef SILKRPC_CORE_REMOTE_BUFFER_HPP_
#define SILKRPC_CORE_REMOTE_BUFFER_HPP_

#include <iostream>
#include <optional>

#include <silkrpc/config.hpp> // NOLINT(build/include_order)

#include <asio/awaitable.hpp>
#include <evmc/evmc.hpp>
#include <silkworm/common/util.hpp>
#include <silkworm/state/buffer.hpp>

namespace silkrpc {

class RemoteBuffer : public silkworm::StateBuffer {
public:
    explicit RemoteBuffer() {}

    std::optional<silkworm::Account> read_account(const evmc::address& address) const noexcept override;

    silkworm::Bytes read_code(const evmc::bytes32& code_hash) const noexcept override;

    evmc::bytes32 read_storage(const evmc::address& address, uint64_t incarnation, const evmc::bytes32& location) const noexcept override;

    uint64_t previous_incarnation(const evmc::address& address) const noexcept override;

    std::optional<silkworm::BlockHeader> read_header(uint64_t block_number, const evmc::bytes32& block_hash) const noexcept override;

    std::optional<silkworm::BlockBody> read_body(uint64_t block_number, const evmc::bytes32& block_hash) const noexcept override;

    std::optional<intx::uint256> total_difficulty(uint64_t block_number, const evmc::bytes32& block_hash) const noexcept override;

    evmc::bytes32 state_root_hash() const override;

    uint64_t current_canonical_block() const override;

    std::optional<evmc::bytes32> canonical_hash(uint64_t block_number) const override;

    void insert_block(const silkworm::Block& block, const evmc::bytes32& hash) override {}

    void canonize_block(uint64_t block_number, const evmc::bytes32& block_hash) override {}

    void decanonize_block(uint64_t block_number) override {}

    void insert_receipts(uint64_t block_number, const std::vector<silkworm::Receipt>& receipts) override {}

    void begin_block(uint64_t block_number) override {}

    void update_account(
        const evmc::address& address,
        std::optional<silkworm::Account> initial,
        std::optional<silkworm::Account> current
    ) override {}

    void update_account_code(
        const evmc::address& address,
        uint64_t incarnation,
        const evmc::bytes32& code_hash,
        silkworm::ByteView code
    ) override {}

    void update_storage(
        const evmc::address& address,
        uint64_t incarnation,
        const evmc::bytes32& location,
        const evmc::bytes32& initial,
        const evmc::bytes32& current
    ) override {}

    void unwind_state_changes(uint64_t block_number) override {}

private:
    silkworm::Bytes db_get(const std::string& table, const silkworm::ByteView& key) const;
};

std::ostream& operator<<(std::ostream& out, const RemoteBuffer& s);

} // namespace silkrpc

#endif  // SILKRPC_CORE_REMOTE_BUFFER_HPP_
