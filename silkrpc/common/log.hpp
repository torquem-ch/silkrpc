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

#ifndef SILKRPC_COMMON_LOG_H_
#define SILKRPC_COMMON_LOG_H_

#include <absl/strings/string_view.h>

#include <silkworm/common/log.hpp>

namespace silkworm {

bool AbslParseFlag(absl::string_view text, LogLevels* level, std::string* error);
std::string AbslUnparseFlag(LogLevels level);

} // namespace silkworm

namespace silkrpc {

using Logger = silkworm::Logger;
using LogLevel = silkworm::LogLevels;

// LogTrace, LogDebug, LogInfo, LogWarn, LogError, LogCritical, LogNone
#define SILKRPC_TRACE SILKWORM_LOG(silkworm::LogTrace)
#define SILKRPC_DEBUG SILKWORM_LOG(silkworm::LogDebug)
#define SILKRPC_INFO  SILKWORM_LOG(silkworm::LogInfo)
#define SILKRPC_WARN  SILKWORM_LOG(silkworm::LogWarn)
#define SILKRPC_ERROR SILKWORM_LOG(silkworm::LogError)
#define SILKRPC_CRIT  SILKWORM_LOG(silkworm::LogCritical)
#define SILKRPC_LOG   SILKWORM_LOG(silkworm::LogNone)

} // namespace silkrpc

#endif  // SILKRPC_COMMON_LOG_H_