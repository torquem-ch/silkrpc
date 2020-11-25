// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: eth.proto

#include "eth.pb.h"
#include "eth.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace rpcdaemon {

static const char* EthService_method_names[] = {
  "/rpcdaemon.EthService/GetBlock",
  "/rpcdaemon.EthService/GetTransaction",
};

std::unique_ptr< EthService::Stub> EthService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< EthService::Stub> stub(new EthService::Stub(channel));
  return stub;
}

EthService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_GetBlock_(EthService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetTransaction_(EthService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status EthService::Stub::GetBlock(::grpc::ClientContext* context, const ::rpcdaemon::GetBlockRequest& request, ::rpcdaemon::GetBlockResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetBlock_, context, request, response);
}

void EthService::Stub::experimental_async::GetBlock(::grpc::ClientContext* context, const ::rpcdaemon::GetBlockRequest* request, ::rpcdaemon::GetBlockResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetBlock_, context, request, response, std::move(f));
}

void EthService::Stub::experimental_async::GetBlock(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::rpcdaemon::GetBlockResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetBlock_, context, request, response, std::move(f));
}

void EthService::Stub::experimental_async::GetBlock(::grpc::ClientContext* context, const ::rpcdaemon::GetBlockRequest* request, ::rpcdaemon::GetBlockResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetBlock_, context, request, response, reactor);
}

void EthService::Stub::experimental_async::GetBlock(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::rpcdaemon::GetBlockResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetBlock_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rpcdaemon::GetBlockResponse>* EthService::Stub::AsyncGetBlockRaw(::grpc::ClientContext* context, const ::rpcdaemon::GetBlockRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::rpcdaemon::GetBlockResponse>::Create(channel_.get(), cq, rpcmethod_GetBlock_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::rpcdaemon::GetBlockResponse>* EthService::Stub::PrepareAsyncGetBlockRaw(::grpc::ClientContext* context, const ::rpcdaemon::GetBlockRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::rpcdaemon::GetBlockResponse>::Create(channel_.get(), cq, rpcmethod_GetBlock_, context, request, false);
}

::grpc::Status EthService::Stub::GetTransaction(::grpc::ClientContext* context, const ::rpcdaemon::GetTransactionRequest& request, ::rpcdaemon::GetTransactionResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetTransaction_, context, request, response);
}

void EthService::Stub::experimental_async::GetTransaction(::grpc::ClientContext* context, const ::rpcdaemon::GetTransactionRequest* request, ::rpcdaemon::GetTransactionResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetTransaction_, context, request, response, std::move(f));
}

void EthService::Stub::experimental_async::GetTransaction(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::rpcdaemon::GetTransactionResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetTransaction_, context, request, response, std::move(f));
}

void EthService::Stub::experimental_async::GetTransaction(::grpc::ClientContext* context, const ::rpcdaemon::GetTransactionRequest* request, ::rpcdaemon::GetTransactionResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetTransaction_, context, request, response, reactor);
}

void EthService::Stub::experimental_async::GetTransaction(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::rpcdaemon::GetTransactionResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetTransaction_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::rpcdaemon::GetTransactionResponse>* EthService::Stub::AsyncGetTransactionRaw(::grpc::ClientContext* context, const ::rpcdaemon::GetTransactionRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::rpcdaemon::GetTransactionResponse>::Create(channel_.get(), cq, rpcmethod_GetTransaction_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::rpcdaemon::GetTransactionResponse>* EthService::Stub::PrepareAsyncGetTransactionRaw(::grpc::ClientContext* context, const ::rpcdaemon::GetTransactionRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::rpcdaemon::GetTransactionResponse>::Create(channel_.get(), cq, rpcmethod_GetTransaction_, context, request, false);
}

EthService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      EthService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< EthService::Service, ::rpcdaemon::GetBlockRequest, ::rpcdaemon::GetBlockResponse>(
          [](EthService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::rpcdaemon::GetBlockRequest* req,
             ::rpcdaemon::GetBlockResponse* resp) {
               return service->GetBlock(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      EthService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< EthService::Service, ::rpcdaemon::GetTransactionRequest, ::rpcdaemon::GetTransactionResponse>(
          [](EthService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::rpcdaemon::GetTransactionRequest* req,
             ::rpcdaemon::GetTransactionResponse* resp) {
               return service->GetTransaction(ctx, req, resp);
             }, this)));
}

EthService::Service::~Service() {
}

::grpc::Status EthService::Service::GetBlock(::grpc::ServerContext* context, const ::rpcdaemon::GetBlockRequest* request, ::rpcdaemon::GetBlockResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status EthService::Service::GetTransaction(::grpc::ServerContext* context, const ::rpcdaemon::GetTransactionRequest* request, ::rpcdaemon::GetTransactionResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace rpcdaemon
