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

#ifndef SILKRPC_TYPES_FILTER_H_
#define SILKRPC_TYPES_FILTER_H_

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <evmc/evmc.hpp>

namespace silkrpc {

typedef std::vector<evmc::address> FilterAddresses;
typedef std::vector<std::string> FilterSubTopics;
typedef std::vector<FilterSubTopics> FilterTopics;

struct Filter {
    std::optional<uint64_t> from_block;
    std::optional<uint64_t> to_block;
    std::optional<FilterAddresses> addresses;
    std::optional<FilterTopics> topics;
    std::optional<std::string> block_hash;
};

std::ostream& operator<<(std::ostream& out, const std::optional<FilterAddresses>& addresses);

std::ostream& operator<<(std::ostream& out, const std::optional<FilterTopics>& topics);

std::ostream& operator<<(std::ostream& out, const Filter& filter);

} // silkrpc

#endif  // SILKRPC_TYPES_FILTER_H_
