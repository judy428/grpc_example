# grpc_example
grpc test,c++

编译：
    1:使用cmake命令
    2:使用makefile

文件说明：
    client.cc 为完整的客户端使用方法
    server.cc 为完整的服务端使用方法
    asyncClient.cc 为完整的异步客户端使用方法
    server_async.cc 为完整的异步服务端使用方法
    server_async2.cc 为完整的异步服务端使用方法，并且新增了通知队列
    server_async3.cc 为网上的异步服务端使用方法，比较简单
    server_callback.cc 为完整的服务器回调代码
    client_callback.cc 为完整的回调客户端使用方法
    使用回调，对于c++来说，可能更简单，特别是对于需要请求其他服务的时候
    使用同步，对于go来说，更方便
所有的使用方法，可以参考grpc官网：https://github.com/grpc/grpc/tree/master/examples/cpp
其中，回调可以参考：https://github.com/grpc/grpc/tree/master/examples/cpp/route_guide
同步和异步，可以参考：https://github.com/grpc/grpc/tree/master/examples/cpp/helloworld

网上很难找到比较全的grpc的例子，包含同步，异步，回调。就自己尝试着写了一下。
欢迎给出宝贵意见，联系方式 邮箱：zhujy127@163.com 735233657@qq.com
