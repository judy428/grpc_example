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

using namespace std;

//grpc::ClientContext ctx;

void callBackSayHello(::grpc::Status st,helloworld::HelloReply* rep)
{
    if(st.ok()){
        cout<<"recv sayhello callback message:"<<rep->message()<<endl;
    }else{
        cout<<"sayhello callback false"<<endl;
    } 
}

class doSayNiHao :public ::grpc::ClientReadReactor< ::helloworld::HelloReply>{
    public:
        doSayNiHao(helloworld::HelloRequest* req):m_req(req){
        } 
        void startWork()
        {
            StartRead(&m_rep);
            StartCall();
        }
        void OnReadDone(bool ok) override{
            if(ok){
                cout<<"recv sayNiHao callBack msg:"<<m_rep.message()<<endl;
                StartRead(&m_rep);
            }
            // else{
            //     cout<<"sayNihao callBack error"<<endl;
            // }
            
        }
        void OnDone(const Status& s) override {
            if(s.ok()){
                cout<<"sayNiHao finished,ok"<<endl;
            }else{
                cout<<"sayNiHao finished,failed"<<endl;
            }
        }
    public:
        grpc::ClientContext ctx;
        helloworld::HelloRequest* m_req;
    private:
        ::helloworld::HelloReply m_rep;
};

class doSayNiHao2 :public ::grpc::ClientWriteReactor< ::helloworld::HelloRequest>{
    public:
        doSayNiHao2(int64_t times):m_times(times){}
    public:
        void startWork(){
            m_req.set_name("sayNiHao2 " + m_times--);
            StartWrite(&m_req);
            StartCall();
        }
        void OnWriteDone(bool ok) override{
            if(ok){
                if(m_times > 0){
                   m_req.set_name("sayNiHao2 " + std::to_string(m_times--));
                    StartWrite(&m_req); 
                    return;
                }
            }
            StartWritesDone();
        }
        void OnDone(const ::grpc::Status& s) override {
            if(s.ok()){
                cout<<"recv sayNiHao2 call_back,msg:"<<m_rep.message()<<endl;
            }
        }
    public:
        grpc::ClientContext ctx;
        ::helloworld::HelloReply m_rep;
    private:
        int64_t m_times;
        helloworld::HelloRequest m_req;
};

class doSayNiHao3 :public ::grpc::ClientBidiReactor< ::helloworld::HelloRequest,::helloworld::HelloReply>{
    public:
        doSayNiHao3(int64_t times):m_times(times){}
    public:
        void startWork(){
            m_req.set_name("sayNiHao3 " + m_times--);
            StartWrite(&m_req);
            StartRead(&m_rep);
            StartCall();
        }

        void OnWriteDone(bool ok) override{
            if(ok){
                if(m_times > 0){
                   m_req.set_name("sayNiHao3 " + std::to_string(m_times--));
                    StartWrite(&m_req); 
                    return;
                }
            }
            StartWritesDone();
        }
        void OnReadDone(bool ok)override{
            if(ok){
                cout<<"recv sayNiHao3 msg:"<<m_rep.message()<<endl;
                StartRead(&m_rep);
            }
        }
        void OnDone(const ::grpc::Status& s) override {
            if(s.ok()){
              cout<<"sayniHao3 finished"<<endl;  
            }
        }
    public:
        grpc::ClientContext ctx;
    private:
        int64_t m_times;
        helloworld::HelloRequest m_req;
        ::helloworld::HelloReply m_rep;
};

class clientCallback {
    public:
    clientCallback(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}
    void sayHello(grpc::ClientContext& ctx,helloworld::HelloReply* rep){
        helloworld::HelloRequest req;
        req.set_name("sayhello callback");
        
        // cout<<"send SayHello"<<endl;
        auto cfun = std::bind(callBackSayHello,std::placeholders::_1,rep);
        stub_->async()->SayHello(&ctx,&req,rep,cfun);
        // cout<<"send SayHello end"<<endl;
        //std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    
#if 1
    void sayNiHao(doSayNiHao* repStream_){
        //此处的m_req说生命周期，不能小于repStream_->startWork(),因为真正的请求，是在StartCall()里面开始的。可以放在回调里面，或者按照下面的书写也是可以的
        stub_->async()->SayNiHao(&repStream_->ctx,repStream_->m_req,repStream_);
    }
#else
    void sayNiHao(doSayNiHao* repStream_){
        //此处的m_req说生命周期，不能小于repStream_->startWork(),因为真正的请求，是在StartCall()里面开始的。可以放在回调里面，或者按照下面的书写也是可以的
        helloworld::HelloRequest m_req;
        m_req.set_name("saynihao callback");
        stub_->async()->SayNiHao(&repStream_->ctx,&m_req,repStream_);
        repStream_->startWork();
    }
#endif

    void sayNiHao2(doSayNiHao2* streams_){
        stub_->async()->SayNiHao2(&streams_->ctx,&streams_->m_rep,streams_);
    }

    void sayNiHao3(doSayNiHao3* streams_){
        stub_->async()->SayNiHao3(&streams_->ctx,streams_);
    }
    private:
        std::unique_ptr<Greeter::Stub> stub_;
};


int main(int argc, char** argv) {
  // Expect only arg: --db_path=path/to/route_guide_db.json.
  clientCallback guide(grpc::CreateChannel(
      "127.0.0.1:50051", grpc::InsecureChannelCredentials()));

  std::cout << "-------------- sayHello --------------" << std::endl;
  grpc::ClientContext ctx;
  helloworld::HelloReply rep;
  guide.sayHello(ctx,&rep);
  std::cout << "-------------- sayNiHao --------------" << std::endl;
  
  helloworld::HelloRequest req;
  req.set_name("saynihao callback");
  doSayNiHao* repStream_ = new doSayNiHao(&req);
  guide.sayNiHao(repStream_);
  repStream_->startWork();
  std::cout << "-------------- sayNiHao2 --------------" << std::endl;
  doSayNiHao2* streams_ = new doSayNiHao2(5);
  guide.sayNiHao2(streams_);
  streams_->startWork();
  std::cout << "-------------- sayNiHao3 --------------" << std::endl;
  doSayNiHao3* streams3_ = new doSayNiHao3(10);
  guide.sayNiHao3(streams3_);
  streams3_->startWork();

   std::this_thread::sleep_for(std::chrono::seconds(50));
  return 0;
}