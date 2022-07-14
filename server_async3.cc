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


class ServerImpl final
{
public:
    ~ServerImpl()
    {
        server_->Shutdown();
        cq_->Shutdown();
    }

    void Run(string port)
    {
        std::string server_address("127.0.0.1:"+port);

        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service_);

        cq_ = builder.AddCompletionQueue();
        server_ = builder.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        HandleRpcs();
    }

private:
    class CallData
    {
    public:
        CallData(Greeter::AsyncService* service, grpc::ServerCompletionQueue* cq)
            : service_(service)
            , cq_(cq)
            , reader_(&ctx_)
            , status_(CREATE)
            , times_(0)
        {
            Proceed();
        }

        void Proceed()
        {
            if (status_ == CREATE)
            {
                status_ = PROCESS;
                service_->RequestSayNiHao2(&ctx_,  &reader_, cq_, cq_, this);
            }
            else if (status_ == PROCESS)
            {
                // Now that we go through this stage multiple times, 
                // we don't want to create a new instance every time.
                // Refer to gRPC's original example if you don't understand 
                // why we create a new instance of CallData here.
                if (times_ == 0)
                {
                    new CallData(service_, cq_);
                    reader_.Read(&request_, this);
                    cout<<"msg:"<<request_.name()<<endl;
                    times_++;
                    return;
                }
                std::cout<<times_<<" th server receive request : "<< request_.name() << std::endl;

                if (times_ >= 3)
                {
                    status_ = FINISH;
                    std::string prefix("Hello ");
                    reply_.set_message(prefix + request_.name() + ", no " + std::to_string(times_) );
                    std::cout<<"read finish!!!"<< std::endl;
                    reader_.Finish(reply_,Status::OK, this);
                }
            	else
            	{
                    // read one more
                    ++times_;
                    reader_.Read(&request_, this);
                }
            }
            else
            {
                std::cout<<"delete this!!!"<< std::endl;
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
        }

    private:
        Greeter::AsyncService* service_;
        grpc::ServerCompletionQueue* cq_;
        ServerContext ctx_;

        HelloRequest request_;
        HelloReply reply_;

        ::grpc::ServerAsyncReader<HelloReply, HelloRequest> reader_;

        int times_;

        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status_; // The current serving state.
    };

    void HandleRpcs()
    {
        new CallData(&service_, cq_.get());
        void* tag; // uniquely identifies a request.
        bool ok;
        while (true)
        {
            GPR_ASSERT(cq_->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData*>(tag)->Proceed();
        }
    }

    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    Greeter::AsyncService service_;
    std::unique_ptr<Server> server_;
};

int main(int argc, char** argv)
{
    ServerImpl server;

    server.Run("50051");

    return 0;
}