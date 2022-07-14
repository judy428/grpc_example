#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <grpcpp/grpcpp.h>
 
#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif
 
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
 
// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    std::cout<<"recv sayHello msg:"<<request->name()<<std::endl;
    reply->set_message(prefix + request->name());
    return Status::OK;
  }

   Status SayNiHao(ServerContext* context, const HelloRequest* request,
                  grpc::ServerWriter< ::helloworld::HelloReply>* rel) override {
    
    HelloReply rp;
    rp.set_message("hello,nihao");
    for (size_t i = 0; i < 5000; i++)
    {
      /* code */
      std::cout<<"hello"<<std::endl;
      rp.set_message("hello,nihao" + std::to_string(i));
      rel->Write(rp);
    }
    return Status::OK;
  }

  Status SayNiHao2(::grpc::ServerContext* context, ::grpc::ServerReader< ::helloworld::HelloRequest>* reader, ::helloworld::HelloReply* response) override {
    HelloRequest req;
    while (reader->Read(&req))
    {
      /* code */
      std::cout<<"read data from client:"<<req.name()<<std::endl;
    }
    response->set_message("hell nihao2");
    return Status::OK;
  }

  Status SayNiHao3(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::helloworld::HelloReply, ::helloworld::HelloRequest>* stream) override{
    HelloRequest req;
    while (stream->Read(&req))
    {
      /* code */
      std::cout<<"recv msg from SayNiHao3:"<<req.name()<<std::endl;
      HelloReply rsp;
      rsp.set_message("rel_" +  req.name());
      stream->Write(rsp);
    }
    return Status::OK;
  }
};
 

std::unique_ptr<Server> rs;

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;
 
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  rs = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;
 
  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  rs->Wait();
}
 
int main(int argc, char** argv) {
  
  std::thread th(RunServer);
  //auto rs = RunServer();
  std::this_thread::sleep_for(std::chrono::seconds(1000));
  rs->Shutdown();
  th.join();
  return 0;
}