#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <thread>
#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif
 
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;
 
class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}
 
  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);
 
    // Container for the data we expect from the server.
    HelloReply reply;
 
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
 
    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);
 
    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }
 
 std::string SayNiHao(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    std::cout<<"SayNiHao send msg:"<<user<<std::endl;
    request.set_name(user);
 
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    HelloReply reply;
    // The actual RPC.
    //auto reply = stub_->SayNiHao(&context, request);

    std::unique_ptr< ::grpc::ClientReaderInterface< ::helloworld::HelloReply>> reader(
        stub_->SayNiHao(&context, request));
    while (reader->Read(&reply)) {
      std::cout << reply.message()<< std::endl;
    }
    Status status = reader->Finish();
    if (status.ok()) {
      std::cout << "ListFeatures rpc succeeded." << std::endl;
       return "RPC successful";
    } else {
      std::cout << "ListFeatures rpc failed." << std::endl;
      return "RPC failed";
    }
  }
 
  std::string SayNiHao2(std::vector<std::string>& reqs){

    ClientContext context;
    HelloReply reply;
    std::unique_ptr< ::grpc::ClientWriterInterface< ::helloworld::HelloRequest>> writer(
                stub_->SayNiHao2(&context, &reply));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (auto &req :reqs)
    {
      /* code */
      HelloRequest request;
      request.set_name(req);
      if(!writer->Write(request)){
        std::cout<<"SayNiHao2 write error"<<std::endl;
        return "error";
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    writer->WritesDone();
    std::cout<<"WritesDone"<<std::endl;
    grpc::Status status = writer->Finish();
    if (!status.ok()) {
        std::cout << "status error: " << status.error_message() << std::endl;
        return "error";
    }else{
      std::cout<<"status ok:msg:"<<reply.message()<<std::endl;
      return "successful";
    }
      
  }

  std::string SayNiHao3(std::vector<std::string>& reqs){
    ClientContext context;
    
    std::shared_ptr< ::grpc::ClientReaderWriter< ::helloworld::HelloRequest, ::helloworld::HelloReply>> stream(
                stub_->SayNiHao3(&context));

    //std::thread reqt([=](std::unique_ptr< ::grpc::ClientReaderWriter< ::helloworld::HelloRequest, ::helloworld::HelloReply>> stream,std::vector<std::string>& reqs){
     std::thread reqt([stream,&reqs](){
      for (auto &req :reqs)
      {
        /* code */
        HelloRequest request;
        request.set_name(req);
        if(!stream->Write(request)){
          std::cout<<"SayNiHao3 write error"<<std::endl;
          return "error";
        }
        std::cout<<"write successful"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
      };
      stream->WritesDone();
      return "successful";
    });
    
    
    HelloReply reply;
    while (stream->Read(&reply))
    {
      /* code */
      std::cout<<"SayNiHao3:recv msg"<<reply.message()<<std::endl;
      // std::cout<<"write successful"<<std::endl;
    }
    reqt.join();
    grpc::Status status = stream->Finish();
    if (!status.ok()) {
        std::cout << "status error: " << status.error_message() << std::endl;
        return "error";
    }else{
      return "successful";
    }

  }
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};
 
int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated

  // (use of InsecureChannelCredentials()).
  GreeterClient greeter(grpc::CreateChannel(
      "127.0.0.1:50051", grpc::InsecureChannelCredentials()));
  
#if 1
  std::string reply = greeter.SayHello("******************SayHello");
  std::cout << "Greeter received SayHello: " << reply << std::endl;
 #endif 
  std::string user("world");
  std::string reply2 = greeter.SayNiHao(user);
  std::cout << "Greeter received SayNiHao: " << reply2 << std::endl;

  

  std::vector<std::string> req = {"1","2","3","4"};
  std::string reply22 = greeter.SayNiHao2(req);
  std::cout << "Greeter received SayNiHao2: " << reply22 << std::endl;

  std::vector<std::string> req2 = {"5","6","7","8"};
  std::string reply3 = greeter.SayNiHao3(req2);
  std::cout << "Greeter received SayNiHao3: " << reply3 << std::endl;

  return 0;
}