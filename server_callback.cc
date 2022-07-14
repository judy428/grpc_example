#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

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
using grpc::ServerUnaryReactor;

using namespace std;
// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::CallbackService {
  #if 0
    //两种方法，都能实现回调
  virtual ::grpc::ServerUnaryReactor* SayHello(
      ::grpc::CallbackServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response) override{
    cout<<"request:"<<request->name()<<endl;
    std::string prefix("Hello ");
    response->set_message(prefix + request->name());
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(Status::OK);
    return reactor;
  }

#else
   virtual ::grpc::ServerUnaryReactor* SayHello(
      ::grpc::CallbackServerContext* context, const ::helloworld::HelloRequest* request, ::helloworld::HelloReply* response) override{
    
    class doSayHello :public ServerUnaryReactor{
        public:
        doSayHello(::helloworld::HelloReply* rep):m_rep(rep){}
        void wirteMsg(){
          std::string prefix("sayHello");
          m_rep->set_message(prefix);
          Finish(Status::OK);
        }

        void OnDone(){
          delete this;
        }
      private:
        ::helloworld::HelloReply* m_rep;
    };
    cout<<"recv sayHello msg:"<<request->name()<<endl;
    
    auto ds = new doSayHello(response);
    ds->wirteMsg();
    return ds;
    
  }
#endif
  virtual ::grpc::ServerWriteReactor< ::helloworld::HelloReply>* SayNiHao(
      ::grpc::CallbackServerContext* context, const ::helloworld::HelloRequest* request) override{

        class sendNiHao :public ::grpc::ServerWriteReactor< ::helloworld::HelloReply>{
          public:
            sendNiHao(const ::helloworld::HelloRequest* request,std::vector<int>& val):m_request(request),m_val(val){
              next_future_ = m_val.begin();
              sendmsg();
            }
          void OnDone() override { delete this; }
          void OnWriteDone(bool /*ok*/) override { sendmsg(); }
          private:
            void sendmsg(){
              
              if(next_future_ != m_val.end()){
                rep.set_message("sayNiHao " + std::to_string(*next_future_));
                StartWrite(&rep);
                ++next_future_;
                return;
              }
              
              Finish(::grpc::Status::OK);
            }

            const std::vector<int> m_val;
            const ::helloworld::HelloRequest* m_request;
            // const std::vector<string>::const_iterator next_future_;
            std::vector<int>::const_iterator next_future_;
            ::helloworld::HelloReply rep;
        };
        cout<<"saynihao recv request:"<<request->name()<<endl;
       std::vector<int> m{4,5,6};
        return new sendNiHao(request,m);
      }

  virtual ::grpc::ServerReadReactor< ::helloworld::HelloRequest>* SayNiHao2(
      ::grpc::CallbackServerContext* context, ::helloworld::HelloReply* response) override{
        class RecvMsg :public ::grpc::ServerReadReactor< ::helloworld::HelloRequest>{
          public:
            RecvMsg(::helloworld::HelloReply* response):m_rep(response){
              StartRead(&m_req);
            }
            void OnDone(){delete this;}
            void OnReadDone(bool ok){
              if(ok){
                cout<<"recv sayNiHao2:"<<m_req.name()<<endl;
                StartRead(&m_req);
              }else{
                m_rep->set_message("sayNiHao2 test");
                Finish(::grpc::Status::OK);
              }
            }
          private:
          ::helloworld::HelloRequest m_req;
          ::helloworld::HelloReply* m_rep;
        };

        // response->set_message("sayNiHao2");

        return new RecvMsg(response);
      }

  virtual ::grpc::ServerBidiReactor< ::helloworld::HelloRequest, ::helloworld::HelloReply>* SayNiHao3(
      ::grpc::CallbackServerContext* context) override{
#if 0
        class RecvSednMsg :public ::grpc::ServerBidiReactor< ::helloworld::HelloRequest, ::helloworld::HelloReply>{
          public:
            RecvSednMsg(std::vector<int>& val):m_val(val){
              m_next = m_val.begin();
              // while (m_next != m_val.end())
              // {
              //   cout<<"val:"<<*m_next <<endl;
              //   m_next++;
              // }
              m_next = m_val.begin();
              cout<<"end"<<endl;
              StartRead(&m_req);
            }
            void OnDone() override{delete this;}
            void OnReadDone(bool ok){
              if(ok){
                cout<<"recv sayNiHao3:"<<m_req.name()<<endl;
                StartRead(&m_req);
              }else{
                if(m_next != m_val.end()){
                  cout<<"val:"<<*m_next<<endl;
                  m_rep.set_message("sayNiHao3 " + std::to_string(*m_next));
                  StartWrite(&m_rep);
                  m_next++;
                }else{
                  Finish(::grpc::Status::OK);
                }
              }
            }
            void OnWriteDone(bool ok) override{
              if(m_next != m_val.end()){
                  cout<<"val:"<<*m_next<<endl;
                  m_rep.set_message("sayNiHao3 " + std::to_string(*m_next));
                  StartWrite(&m_rep);
                  m_next++;
                }else{
                  Finish(::grpc::Status::OK);
                }
            }
          private:
            ::helloworld::HelloRequest m_req;
            ::helloworld::HelloReply m_rep;
            std::vector<int>::const_iterator m_next;
            std::vector<int> m_val;
            //int m_pos;
        };
#else
      class RecvSednMsg :public ::grpc::ServerBidiReactor< ::helloworld::HelloRequest, ::helloworld::HelloReply>{
          public:
            RecvSednMsg(std::vector<int>& val):m_val(val){
              m_next = m_val.begin();
              StartRead(&m_req);

              if(m_next != m_val.end()){
                  m_rep.set_message("sayNiHao3 " + std::to_string(*m_next));
                  StartWrite(&m_rep);
                  m_next++;
                }
            }
            void OnDone() override{
              
              delete this;
            }
            void OnReadDone(bool ok){
              if(ok){
                cout<<"recv sayNiHao3:"<<m_req.name()<<endl;
                StartRead(&m_req);
              }
            }
            void OnWriteDone(bool ok) override{
              if(m_next != m_val.end()){
                  m_rep.set_message("sayNiHao3 " + std::to_string(*m_next));
                  StartWrite(&m_rep);
                  m_next++;
                }else{
                  Finish(::grpc::Status::OK);
                }
            }
          private:
            ::helloworld::HelloRequest m_req;
            ::helloworld::HelloReply m_rep;
            std::vector<int>::const_iterator m_next;
            std::vector<int> m_val;
            //int m_pos;
        };
#endif
        std::vector<int> val{9,10,11,12};
        return new RecvSednMsg(val);

      }

#if 0
  virtual ::grpc::ServerBidiReactor< ::helloworld::HelloRequest, ::helloworld::HelloReply>* SayNiHao3(
      ::grpc::CallbackServerContext* context) override{
        class RecvSednMsg :public ::grpc::ServerBidiReactor< ::helloworld::HelloRequest, ::helloworld::HelloReply>{
          public:
            RecvSednMsg(std::vector<int>& val):m_val(val),m_pos(0){
              while (m_pos != m_val.size())
              {
                cout<<"val:"<<m_val[m_pos] <<endl;
                m_pos++;
              }
              m_pos = 0;
              cout<<"end"<<endl;
              StartRead(&m_req);
            }
            void OnDone() override{delete this;}
            void OnReadDone(bool ok){
              if(ok){
                cout<<"recv sayNiHao3:"<<m_req.name()<<endl;
                StartRead(&m_req);
              }else{
                if(m_pos < m_val.size()){
                  cout<<"val:"<<m_val[m_pos]<<endl;
                  m_rep.set_message("sayNiHao3 " + std::to_string(m_val[m_pos]));
                  StartWrite(&m_rep);
                  m_pos++;
                }else{
                  Finish(::grpc::Status::OK);
                }
              }
            }
            void OnWriteDone(bool ok) override{
              if(m_pos < m_val.size()){
                  cout<<"val:"<<m_val[m_pos]<<endl;
                  m_rep.set_message("sayNiHao3 " + std::to_string(m_val[m_pos]));
                  StartWrite(&m_rep);
                  m_pos++;
                }else{
                  Finish(::grpc::Status::OK);
                }
            }
          private:
            ::helloworld::HelloRequest m_req;
            ::helloworld::HelloReply m_rep;
            //std::vector<int>::const_iterator m_next;
            std::vector<int> m_val;
            int m_pos;
        };
        std::vector<int> val{9,10,11,12};
        return new RecvSednMsg(val);

      }

  WithCallbackMethod_SayHello
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
  #endif
};
 

void RunServer() {
  std::string server_address("127.0.0.1:50051");
  GreeterServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}