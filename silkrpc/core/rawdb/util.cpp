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

#include "util.hpp"
#include <cstdlib>
#include <cstring>


#include <boost/endian/conversion.hpp>
#include <intx/int128.hpp>

#include <silkworm/common/base.hpp>
#include <silkworm/common/util.hpp>
#include <silkworm/db/util.hpp>

namespace silkrpc {

silkworm::Bytes generate_composity_storage_key(const evmc::address& address, uint64_t incarnation, const uint8_t (&hash)[silkworm::kHashLength]) {
    silkworm::Bytes res(silkworm::db::kStoragePrefixLength + silkworm::kHashLength, '\0');
    std::memcpy(&res[0], address.bytes, silkworm::kAddressLength);
    boost::endian::store_big_u64(&res[silkworm::kAddressLength], incarnation);
    std::memcpy(&res[silkworm::db::kStoragePrefixLength], hash, silkworm::kHashLength);
    return res;
}

} // namespace silkrpc
