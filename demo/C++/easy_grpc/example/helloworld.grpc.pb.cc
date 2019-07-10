// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: helloworld.proto

#include "helloworld.pb.h"
#include "helloworld.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace helloworld {

static const char* Greeter_method_names[] = {
  "/helloworld.Greeter/SayHello",
};

std::unique_ptr< Greeter::Stub> Greeter::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Greeter::Stub> stub(new Greeter::Stub(channel));
  return stub;
}

Greeter::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_SayHello_(Greeter_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Greeter::Stub::SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::helloworld::HelloReply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SayHello_, context, request, response);
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helloworld::HelloReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, std::move(f));
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, reactor);
}

void Greeter::Stub::experimental_async::SayHello(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helloworld::HelloReply* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_SayHello_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::helloworld::HelloReply>* Greeter::Stub::AsyncSayHelloRaw(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::helloworld::HelloReply>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::helloworld::HelloReply>* Greeter::Stub::PrepareAsyncSayHelloRaw(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::helloworld::HelloReply>::Create(channel_.get(), cq, rpcmethod_SayHello_, context, request, false);
}

Greeter::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Greeter_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Greeter::Service, ::helloworld::HelloRequest, ::helloworld::HelloReply>(
          std::mem_fn(&Greeter::Service::SayHello), this)));
}

Greeter::Service::~Service() {
}

::grpc::Status Greeter::Service::SayHello(::grpc::ServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace helloworld

