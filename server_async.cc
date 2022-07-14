//note::
/*
1:对于流来说,read与write都是异步,意思是:stream.read()或者stream.write()是异步,真正的结束,需要完成队列给出消息通知,因此，给到read的对象，必须不能是局部变量，负责会被析构，报错，
报错内如为：pure virtual method called
terminate called without an active exception
也因为如此，每一次事件通知（即一个while循环），只能放一个处理在里面，否则无法知道新接受的事件，对应哪一个处理。
2:对于读取流，存放进read的变量，一定不能是局部变量，
  对于写入流，则可以
3:循环时，ok为false，可能是读入流读取完成，可以直接删除
4：读取流什么时候结束，需要再看看(结束时Read()时，next返回false，说明文档：https://grpc.github.io/grpc/cpp/classgrpc_1_1_completion_queue.html#a86d9810ced694e50f7987ac90b9f8c1a)
5：注册的时候，有两个队列的时候，第一个队列，只接受新的链接，第二个队列处理链接的具体工作

6:
Client发送流，是通过Writer->WritesDone()函数结束流；


Server发送流，是通过结束RPC函数并返回状态码的方式来结束流；



流接受者，都是通过Reader->Read()返回的bool型状态，来判断流是否结束。
*/



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
 

using namespace std;

enum{
  NewConnect = 0,
  Reading,
  Writing,
  Finished
};

struct HandlerContextBase {
  // 当前对象类型，用于确定是Test1/2/3哪一个请求的
  int                 type_;
  // 当前处理状态（处理分为两步：1处理请求构建响应数据；2发送响应）
  // 这里记录一下完成到哪一步了，以便进行相关操作
  int                 status_; // (1构建响应完成；2发送完成)
  // rpc的上下文，允许通过它进行诸如压缩、身份验证，以及把元数据发回客户端等。
  
  int requestid;
  grpc::ServerContext ctx_{};
  virtual ~HandlerContextBase()
  {
    std::cout<<"delete HandlerContextBase"<<std::endl;
  }
};
 
template<typename RequestType,typename ReplyType>
struct HandlerDirect:public HandlerContextBase {
  // 用于接收客户端发送的请求
  RequestType         req_;
  // 用于发送响应给客户端
  ReplyType           rep_;
  // 发送到客户端的方法对象
  grpc::ServerAsyncResponseWriter<ReplyType> responder_;
  //================================================
  // 构造函数
  HandlerDirect()
    :responder_(&ctx_)
  {}
};

template<typename RequestType,typename ReplyType>
struct HandlerDirectStream:public HandlerContextBase {
  // 用于接收客户端发送的请求
  int retValue = 0;
  RequestType         req_;
  // 用于发送响应给客户端
  //grpc::ServerWriter< ReplyType> rep_;
  // 发送到客户端的方法对象
  ::grpc::ServerAsyncWriter< ReplyType> stream_;
  int readFlag;//0:新建立请求，可以读，1：已经读取完成，做其他动作
  //================================================
  // 构造函数
  HandlerDirectStream()
    :stream_(&ctx_),readFlag(0)
  {}
};

template<typename RequestType,typename ReplyType>
struct HandlerStreamDirect:public HandlerContextBase {
  RequestType         req_;
  // 发送到客户端的方法对象
  ::grpc::ServerAsyncReader< ReplyType, RequestType> responder_;
  //================================================
  // 构造函数
  int newCall;
  HandlerStreamDirect()
    :responder_(&ctx_),newCall(0)
  {}
  virtual ~HandlerStreamDirect()
  {
    std::cout<<"delete HandlerStreamDirect"<<std::endl;
  }
};

template<typename RequestType,typename ReplyType>
struct HandlerStreamStream:public HandlerContextBase {
  int retValue = 0;
  RequestType         req_;
  // 发送到客户端的方法对象
  ::grpc::ServerAsyncReaderWriter< ReplyType , RequestType> stream_;
  int readWriteFlag;
  //================================================
  // 构造函数
  HandlerStreamStream()
    :stream_(&ctx_),readWriteFlag(0)
  {}
};

typedef HandlerDirect<::helloworld::HelloRequest,::helloworld::HelloReply>  HandlerDirectReal;
typedef HandlerDirectStream<::helloworld::HelloRequest,::helloworld::HelloReply>  HandlerDirectStreamReal;
typedef HandlerStreamDirect<::helloworld::HelloRequest,::helloworld::HelloReply>  HandlerStreamDirectReal;
typedef HandlerStreamStream<::helloworld::HelloRequest,::helloworld::HelloReply>  HandlerStreamStreamReal;

  void SayHello( HandlerDirectReal* ins)  {
    std::string prefix("Hello ");
    std::cout<<"SayHello recv msg:"<<ins->req_.name()<<std::endl;
    ins->rep_.set_message(prefix + ins->req_.name());

    ins->status_ = 2;
    ins->responder_.Finish(ins->rep_,grpc::Status::OK,ins);
  }

  
  void DoSayNiHao(HandlerDirectStreamReal* ins){
    //cout<<"call DoSayNiHao"<<endl;
    
    if(ins->readFlag == 0){
      cout<<"DoSayNiHao recv msg:"<<ins->req_.name()<<endl;
      ins->readFlag = 1;
    }

    if(ins->retValue > 10){
      ins->status_ = 2;
      ins->stream_.Finish(::grpc::Status::OK,(void*)ins);
      return;
    }
    ::helloworld::HelloReply rep;
    rep.set_message("return value:" + std::to_string(++ins->retValue));
     
    //cout<<"write msg DoSayNiHao,begin:"<<endl;
    ins->stream_.Write(rep,(void*)ins);
    //cout<<"write msg DoSayNiHao,end:"<<endl;
  }

  void DoSayNiHao3Read(HandlerStreamStreamReal* ins){
   
      if(ins->readWriteFlag == 0){
        ins->readWriteFlag = 1;
      }else{
        std::cout<<"DoSayNiHao3 recv msg:"<<ins->req_.name()<<std::endl;
      }
      if(ins->readWriteFlag = 1){
        ins->stream_.Read(&ins->req_,(void*)ins);
      }
  }
    
    void DoSayNiHao3Write(HandlerStreamStreamReal* ins)
    {
      ::helloworld::HelloReply rep;
      if(ins->retValue++ > 7){
        ins->status_ = 2;
        ins->stream_.Finish(grpc::Status::OK,ins);
      }else{
        rep.set_message("return value:" + std::to_string(ins->retValue));
        ins->readWriteFlag = ins->readWriteFlag | 4;
        ins->stream_.Write(rep,(void*)ins);
      }
      
    }
    //


int main()
{
  grpc::ServerBuilder builder;
  builder.AddListeningPort("127.0.0.1:50051",grpc::InsecureServerCredentials());
  helloworld::Greeter::AsyncService Aservice;
  builder.RegisterService(&Aservice);

  std::unique_ptr<grpc::ServerCompletionQueue> cq_new_ptr = builder.AddCompletionQueue();
  //std::unique_ptr<grpc::ServerCompletionQueue> cq_notify_ptr = builder.AddCompletionQueue();

  std::unique_ptr<grpc::Server> AsyncServer(builder.BuildAndStart());
  std::cout<<"Async Server Running"<<std::endl;

  HandlerDirectReal *h1 = new HandlerDirectReal;
  h1->status_ = 1;
  h1->type_ = 1;

  HandlerDirectStreamReal *h2 = new HandlerDirectStreamReal;
  h2->status_ = 1;
  h2->type_ = 2;

  HandlerStreamDirectReal *h3 = new HandlerStreamDirectReal;
  h3->status_ = 1;
  h3->type_ = 3;

  HandlerStreamStreamReal *h4 = new HandlerStreamStreamReal;
  h4->status_ = 1;
  h4->type_ = 4;

  Aservice.RequestSayHello(&(h1->ctx_),&h1->req_,&h1->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h1);
  // h2->ctx_.AsyncNotifyWhenDone(h2);
  Aservice.RequestSayNiHao(&h2->ctx_,&h2->req_,&h2->stream_,cq_new_ptr.get(),cq_new_ptr.get(),h2);
  //h3->ctx_.AsyncNotifyWhenDone(h3);
  Aservice.RequestSayNiHao2(&h3->ctx_,&h3->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h3);
  //  h4->ctx_.AsyncNotifyWhenDone(h4);
  Aservice.RequestSayNiHao3(&(h4->ctx_),&(h4->stream_),cq_new_ptr.get(),cq_new_ptr.get(),h4);

#if 1
  std::thread th1([&] {
    
    /* code */
    HandlerContextBase * hcb;
    bool okFlag = false;
    while(cq_new_ptr->Next((void**)&hcb,&okFlag)){
      //cout<<"hcb->type_:"<<hcb->type_<<endl;
      
      std::cout<<"okflag:"<<okFlag<<std::endl;
      // if(okFlag == 0){
      //   if(hcb != nullptr){
      //     cout<<"okFlag is false ,delete hcb:type"<<hcb->type_<<endl;
      //     delete hcb;
      //     continue;
      //   }
      // }
      if(hcb == nullptr){
        cout<<"read nullptr"<<endl;
        continue;
      }
      if(hcb == nullptr)
      {
        cout<<"hcb == nullptr"<<endl;
      }
      if(hcb->status_ == 2){
        cout<<"delete hcb:type"<<hcb->type_<<endl;
        
        //cout<<"status code:"<<hcb->status_.error_code()<<",msg:"<<hcb->status_.error_message()<<endl;
        delete hcb;
        continue;
      }
      
      switch (hcb->type_)
      {
      case 1:
        {
          HandlerDirectReal *h1 = new HandlerDirectReal;
          h1->status_ = 1;
          h1->type_ = 1;
          Aservice.RequestSayHello(&h1->ctx_,&h1->req_,&h1->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h1);

          HandlerDirectReal* ins = (HandlerDirectReal*)hcb;
         SayHello(ins);
          
        }
        break;
      
      case 2:
        {
          HandlerDirectStreamReal* ins = (HandlerDirectStreamReal*)hcb;
          if(ins->readFlag == 0){
            cout<<"new snh"<<endl;
            HandlerDirectStreamReal *h2 = new HandlerDirectStreamReal;
            h2->status_ = 1;
            h2->type_ = 2;
            Aservice.RequestSayNiHao(&h2->ctx_,&h2->req_,&h2->stream_,cq_new_ptr.get(),cq_new_ptr.get(),h2);
          }
          
          DoSayNiHao(ins);
        }
        break;
      case 3:
        
        {
          
          HandlerStreamDirectReal* ins = static_cast<HandlerStreamDirectReal*>(hcb);
          // if(ins->ctx_.IsCancelled()){
          //     cout<<"snh2 is canceled"<<endl;
          // }
          if(okFlag){
            
            if(ins->newCall == 0){
              cout<<"new snh2"<<endl;
              HandlerStreamDirectReal *h = new HandlerStreamDirectReal;
              h->status_ = 1;
              h->type_ = 3;
              Aservice.RequestSayNiHao2(&h->ctx_,&h->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h);

              ins->newCall = 1;
            }else{
              cout<<"DoSayNiHao2 recv msg:"<<ins->req_.name()<<endl;
            }
            // HelloRequest req_;
            // ins->responder_.Read(&req_,ins);
            // if(!ins->ctx_.IsCancelled() ){
            //   ins->responder_.Read(&ins->req_,ins);
            // }else{
            //   cout<<" saynihao2 recv canceled,send finished"<<endl;
            //   ::helloworld::HelloReply rep;
            //   rep.set_message("DoSayNiHao2 finished");
            //   ins->responder_.Finish(rep,grpc::Status::OK,ins);
            // }
             ins->responder_.Read(&ins->req_,ins); 
          }else{
            // if(ins->ctx_.IsCancelled()){
            //   cout<<"snh2 is canceled,delete"<<endl;
            //   delete ins;
            //   continue;
            // }else{
            //   ::helloworld::HelloReply rep;
            //   rep.set_message("DoSayNiHao2 finished");
            //   ins->status_ = 2;
            //   ins->responder_.Finish(rep,grpc::Status::OK,ins);
            // }
           ::helloworld::HelloReply rep;
              rep.set_message("DoSayNiHao2 finished");
              ins->status_ = 2;
              ins->responder_.Finish(rep,grpc::Status::OK,ins);
          }
          
              //DoSayNiHao2(ins);
            
        }
        
        break;

      case 4:
      {
        //status 
        //0:new connected
        //1:正在读
        //5:读完成
        //3:正在写
        //4:写完成
        HandlerStreamStreamReal* ins = (HandlerStreamStreamReal*)hcb;
        if(okFlag == 0){
          
          //表示读写已经结束
          ins->status_ = 5;
          // ins->stream_.Finish(::grpc::Status::OK,ins);
          // continue;
        }
        
        if(ins->readWriteFlag == 0){
          cout<<"register new func4"<<endl;
          HandlerStreamStreamReal *h = new HandlerStreamStreamReal;
          h->status_ = 1;
          h->type_ = 4;
          Aservice.RequestSayNiHao3(&(h->ctx_),&(h->stream_),cq_new_ptr.get(),cq_new_ptr.get(),h);
        }
        //同一个事件,只能read或者write,不能同时进行
        
        if(ins->status_ != 5){
          DoSayNiHao3Read(ins);
        }else{
          DoSayNiHao3Write(ins);
        }
        //
      }
      break;
      default:
        break;
      }
    }
    
  });

#endif

#if 0
  {
    /* code */
    HandlerContextBase * hcb;
    bool okFlag = false;
    while(cq_new_ptr->Next((void**)&hcb,&okFlag)){
      //cout<<"hcb->type_:"<<hcb->type_<<endl;
      
      std::cout<<"okflag:"<<okFlag<<std::endl;
      // if(okFlag == 0){
      //   if(hcb != nullptr){
      //     cout<<"okFlag is false ,delete hcb:type"<<hcb->type_<<endl;
      //     delete hcb;
      //     continue;
      //   }
      // }
      if(hcb == nullptr){
        cout<<"read nullptr"<<endl;
        continue;
      }
      if(hcb->status_ == 2){
        cout<<"delete hcb:type"<<hcb->type_<<endl;
        delete hcb;
        continue;
      }
      if(hcb == nullptr)
      {
        cout<<"hcb == nullptr"<<endl;
      }
      
      switch (hcb->type_)
      {
      case 1:
        {
          HandlerDirectReal *h1 = new HandlerDirectReal;
          h1->status_ = 1;
          h1->type_ = 1;
          Aservice.RequestSayHello(&h1->ctx_,&h1->req_,&h1->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h1);

          HandlerDirectReal* ins = (HandlerDirectReal*)hcb;
         SayHello(ins);
          
        }
        break;
      
      case 2:
        {
          HandlerDirectStreamReal* ins = (HandlerDirectStreamReal*)hcb;
          if(ins->readFlag == 0){
            cout<<"new snh"<<endl;
            HandlerDirectStreamReal *h2 = new HandlerDirectStreamReal;
            h2->status_ = 1;
            h2->type_ = 2;
            
          }

          
          DoSayNiHao(ins);
        }
        break;
      case 3:
        
        {
          HandlerStreamDirectReal* ins = static_cast<HandlerStreamDirectReal*>(hcb);
          if(okFlag){
            
            if(ins->newCall == 0){
              cout<<"new snh2"<<endl;
              HandlerStreamDirectReal *h = new HandlerStreamDirectReal;
              h->status_ = 1;
              h->type_ = 2;
              Aservice.RequestSayNiHao2(&h->ctx_,&h->responder_,cq_new_ptr.get(),cq_new_ptr.get(),h);

              ins->newCall = 1;
            }else{
              cout<<"DoSayNiHao2 recv msg:"<<ins->req_.name()<<endl;
            }
            // HelloRequest req_;
            // ins->responder_.Read(&req_,ins);
            ins->responder_.Read(&ins->req_,ins);
          }else{
            ::helloworld::HelloReply rep;
            rep.set_message("DoSayNiHao2 finished");
            ins->status_ = 2;
            ins->responder_.Finish(rep,grpc::Status::OK,ins);
          }
          
              //DoSayNiHao2(ins);
            
        }
        
        break;

      case 4:
      {
        //status 
        //0:new connected
        //1:正在读
        //5:读完成
        //3:正在写
        //4:写完成
        HandlerStreamStreamReal* ins = (HandlerStreamStreamReal*)hcb;
        if(okFlag == 0){
          
          //表示读写已经结束
          ins->status_ = 5;
          // ins->stream_.Finish(::grpc::Status::OK,ins);
          // continue;
        }
        
        if(ins->readWriteFlag == 0){
          cout<<"register new func4"<<endl;
          HandlerStreamStreamReal *h = new HandlerStreamStreamReal;
          h->status_ = 1;
          h->type_ = 4;
          Aservice.RequestSayNiHao3(&(h->ctx_),&(h->stream_),cq_new_ptr.get(),cq_new_ptr.get(),h);
        }
        //同一个事件,只能read或者write,不能同时进行
        static int total = 0;
        if(ins->status_ != 5){
          DoSayNiHao3Read(ins);
        }else{
          DoSayNiHao3Write(ins);
        }
        //
      }
      break;
      default:
        break;
      }
    }
    
  }
#endif
  if(th1.joinable()){
    th1.join();
  }
  
  
}