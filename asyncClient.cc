#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include <thread>
#include <chrono>
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
using grpc::CompletionQueue;
using grpc::ClientAsyncResponseReader;

using grpc::ClientAsyncReader;
using grpc::ClientAsyncWriter;
using grpc::ClientReaderWriter;
using grpc::ClientAsyncReaderWriter;


using namespace std;



struct AsyncClientCall {
    // Container for the data we expect from the server.
    int times = 0;
    HelloReply reply;
    HelloRequest request;
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // Storage for the status of the RPC upon completion.
    Status status;
    int func;
    int requestid;
    int status_type;
    std::unique_ptr<ClientAsyncResponseReader<HelloReply>> response_reader;
    std::unique_ptr<ClientAsyncWriter<HelloRequest> > async_writer;
    std::unique_ptr<ClientAsyncReader<HelloReply> >  reader_stream;
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::helloworld::HelloRequest, ::helloworld::HelloReply>> read_write_stream;
    //std::unique_ptr< ::grpc::ClientAsyncReader< ::helloworld::HelloReply>>
  };

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)){}
 
  void SayHello(const std::string& user) {
    // Data we are sending to the server.
    // Container for the data we expect from the server.
   
    AsyncClientCall* asyncC = new AsyncClientCall();
    asyncC->request.set_name(user);
    asyncC->func = 1;
    asyncC->status_type = 2;
    asyncC->response_reader = stub_->AsyncSayHello(&asyncC->context, asyncC->request, &cq_);

    asyncC->response_reader->Finish(&asyncC->reply, &asyncC->status, (void*)asyncC);
  }

  void SayNiHao(const std::string& user) {
    // Data we are sending to the server.

    AsyncClientCall* asyncC = new AsyncClientCall();
    asyncC->request.set_name(user);
    asyncC->func = 2;
    asyncC->requestid = 30;
    asyncC->status_type = 1;
    
    asyncC->reader_stream = stub_->AsyncSayNiHao(&asyncC->context, asyncC->request, &cq_,(void*)asyncC);
    
    //asyncC->reader_stream->Finish( &asyncC->status, (void*)asyncC);

  }
  
  void SayNiHao2(){

    AsyncClientCall* asyncC = new AsyncClientCall();
    asyncC->func = 3;
    asyncC->async_writer = stub_->AsyncSayNiHao2(&asyncC->context,&asyncC->reply, &cq_,(void*)asyncC);
  }

  void DoSayNiHao2(AsyncClientCall* asyncC){
    
    if(asyncC->status_type != 3){
      if(asyncC->times++ < 100){
        asyncC->request.set_name("DoSayNiHao2 " + std::to_string(asyncC->times));
        asyncC->async_writer->Write(asyncC->request,(void*)asyncC);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
      }else{
        asyncC->status_type = 3;//表示已经写结束
        asyncC->async_writer->WritesDone((void*)asyncC);
        
      }
    }else{
      asyncC->status_type = 2;//删除
      
      asyncC->async_writer->Finish(&asyncC->status, (void*)asyncC);
    }
  }
   
  void SayNiHao3(){
    AsyncClientCall* asyncC = new AsyncClientCall();
    asyncC->func = 4;
    asyncC->status_type = 0;
    asyncC->read_write_stream =  stub_->AsyncSayNiHao3(&asyncC->context, &cq_,(void*)asyncC);
  }


  void AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;
    
    // Block until the next result is available in the completion queue "cq".
    int num = 0;
    while (cq_.Next(&got_tag, &ok)) {
      // The tag in this example is the memory location of the call object

      if(got_tag == nullptr){
        continue;
      }
      AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
      if(call == nullptr){
        continue;
      }
      if(!ok){
        cout<<"not ok"<<endl;
        // call->status_type = 3;
        //continue;
      }
      // Verify that the request was completed successfully. Note that "ok"
      // corresponds solely to the request for updates introduced by Finish().

      // GPR_ASSERT(ok);

      if (call->status.ok()){
        if(call->func == 1){
          std::cout << "Greeter received 1: " << call->reply.message() << std::endl;
          delete call;
        }else if(call->func == 2){
          if(ok  ){
            if(call->status_type != 2){
              std::cout << "Greeter received 2: " << call->reply.message() << std::endl;
              call->reader_stream->Read(&call->reply,call);
            }else{
              cout<<"func 2,status_type = 2,delete "<<endl;
              delete call;
              call = nullptr;
              continue;
            }
            
          }else{
            call->status_type = 2;
            call->reader_stream->Finish(&call->status,call);
          }
          
        }else if(call->func == 3){
          if(call->status_type== 2){
            cout<<"DoSayNiHao2 recv msg:"<<call->reply.message()<<endl;
            cout<<"func 3 status_type = 2,delete"<<endl;
            delete call;
          }else{
            DoSayNiHao2(call);
          }
        }else if(call->func == 4){
          //先写，再读
          //1:表示开始写
          //3表示写结束
          //4：表示开始读
          //5：表示读完成
          
          if(!ok){
            cout<<"func 4 is not ok"<<endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            call->status_type = 5;
            call->read_write_stream->Finish(&call->status,call);
            continue;
          }
          if(call->status_type == 0 || call->status_type == 1){
            if(call->times++ < 10){
              call->status_type = 1;
              call->request.set_name("func 4," + std::to_string(call->times));
              call->read_write_stream->Write(call->request,call);
            }else{
              call->status_type = 3;
              call->read_write_stream->WritesDone(call);
            }
            
          }else{
             if (call->status_type == 5)
             {
               cout<<"func 4 finished,delete"<<endl;
               delete call;
               continue;
             }else{
               if(call->status_type == 3){
                  call->status_type = 4;
                }else{
                  cout<<"func 4 recv msg:"<<call->reply.message()<<endl;
                }
                call->read_write_stream->Read(&call->reply,call);
             }
             
             
          }
                  
        }
        // std::cout << "Greeter received 2: " << call->reply.message() << std::endl;
      }else
        std::cout << "RPC failed" << std::endl;

      // Once we're complete, deallocate the call object.
      //delete call;
    }
  }


 private:
  std::unique_ptr<Greeter::Stub> stub_;
  //std::unique_ptr<MultiGreeter::Stub> MultStub_;
  std::unique_ptr<grpc::ClientReaderWriter<HelloRequest,HelloReply> > stream_;
  //std::unique_ptr<ClientAsyncReader<HelloReply> >  reader_stream;
  CompletionQueue cq_;
  // AsyncClientCall* asyncC;
};
 
int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  GreeterClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  // Spawn reader thread that loops indefinitely
  std::thread thread_ = std::thread(&GreeterClient::AsyncCompleteRpc, &greeter);

  for (int i = 0; i < 100; i++) {
    std::string user("world " + std::to_string(i));
    greeter.SayHello(user);  // The actual RPC call!
  }



  greeter.SayNiHao("SayNiHao");
  //std::this_thread::sleep_for(std::chrono::seconds(5));
  greeter.SayNiHao2();
  greeter.SayNiHao3();
  
  

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_.join();  // blocks forever

  return 0;
}