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

#ifndef SILKRPC_ETHDB_KV_ASYNC_START_HPP_
#define SILKRPC_ETHDB_KV_ASYNC_START_HPP_

#include <asio/detail/config.hpp>
#include <asio/detail/bind_handler.hpp>
#include <asio/detail/fenced_block.hpp>
#include <asio/detail/handler_alloc_helpers.hpp>
#include <asio/detail/handler_work.hpp>
#include <asio/detail/memory.hpp>

#include <silkrpc/grpc/async_operation.hpp>

namespace silkrpc::ethdb::kv {

template <typename Handler, typename IoExecutor>
class async_start : public async_operation<void, asio::error_code> {
public:
    ASIO_DEFINE_HANDLER_PTR(async_start);

    async_start(Handler& h, const IoExecutor& io_ex)
    : async_operation(&async_start::do_complete), handler_(ASIO_MOVE_CAST(Handler)(h)), work_(handler_, io_ex)
    {}

    static void do_complete(void* owner, async_operation* base, asio::error_code error = {}) {
        // Take ownership of the handler object.
        async_start* h{static_cast<async_start*>(base)};
        ptr p = {asio::detail::addressof(h->handler_), h, h};

        ASIO_HANDLER_COMPLETION((*h));

        // Take ownership of the operation's outstanding work.
        asio::detail::handler_work<Handler, IoExecutor> work(
            ASIO_MOVE_CAST2(asio::detail::handler_work<Handler, IoExecutor>)(h->work_));

        // Make a copy of the handler so that the memory can be deallocated before
        // the upcall is made. Even if we're not about to make an upcall, a
        // sub-object of the handler may be the true owner of the memory associated
        // with the handler. Consequently, a local copy of the handler is required
        // to ensure that any owning sub-object remains valid until after we have
        // deallocated the memory here.
        asio::detail::binder1<Handler, asio::error_code> handler{h->handler_, error};
        p.h = asio::detail::addressof(handler.handler_);
        p.reset();

        // Make the upcall if required.
        if (owner) {
            asio::detail::fenced_block b(asio::detail::fenced_block::half);
            ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_));
            work.complete(handler, handler.handler_);
            ASIO_HANDLER_INVOCATION_END;
        }
    }

private:
    Handler handler_;
    asio::detail::handler_work<Handler, IoExecutor> work_;
};

} // namespace silkrpc::ethdb::kv

#endif // SILKRPC_ETHDB_KV_ASYNC_START_HPP_
