#ifndef __RPC_SERVICE_CLIENT_H_
#define __RPC_SERVICE_CLIENT_H_

#include <queue>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>





using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

//std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>>


//std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>> 
//PrepareAsyncSayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) 

template<typename K, typename T>
using PrepareFunc = boost::function<std::unique_ptr< ClientAsyncResponseReader< T>> (ClientContext*,const K&,CompletionQueue*)>;


//using PrepareFunc = boost::function<void* (ClientContext*,T&,CompletionQueue*)>;
//typedef boost::function<void* (void)> PrepareFunc;
//typedef boost::function<void* (ClientContext*, CompletionQueue*)> PrepareFunc;
typedef boost::function<void (void)> RpcCbFunc;





class AsyncClientTask {
public:
    void setCb(RpcCbFunc cb) {
        cb_ = cb;
    }
    void callCb() {
        if(!cb_.empty()){
            cb_();
        }
    }
protected:
    RpcCbFunc cb_;
};

template <typename T>
class AsyncClientTaskT: public AsyncClientTask {
public:
    T reply;
    ClientContext context;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<T> > response_reader;
};


void hello();

//one AsyncClient one server
class AsyncClient {
public:
    explicit AsyncClient(std::string addr):server_addr(addr) { start(); }
    ~AsyncClient() {
        stop();
    }

    CompletionQueue &getCompletionQueue() { return cq_;}

    

    template<typename K, typename T>
    int doRpc(const K &req, PrepareFunc<K, T> pf, RpcCbFunc cb = NULL) {
        AsyncClientTaskT<T> *task = new AsyncClientTaskT<T>();
        //call->response_reader.reset(static_cast<ClientAsyncResponseReader<T>*>(pf()));
        task->setCb(cb);
        //task->response_reader = static_cast<ClientAsyncResponseReader<T>*>(pf(&task->context, req, &cq_));
        task->response_reader = pf(&task->context, req, &cq_);
        task->response_reader->StartCall();
        task->response_reader->Finish(&task->reply, &task->status, (void*)task);
        return 0;
    }
#if 0
    template<typename T>
    int doRpc1(PrepareFunc pf, RpcCbFunc cb = NULL) {
        AsyncClientTaskT<T> *task = new AsyncClientTaskT<T>();
        //call->response_reader.reset(static_cast<ClientAsyncResponseReader<T>*>(pf()));
        task->setCb(cb);
        task->response_reader = static_cast<ClientAsyncResponseReader<T>*>(pf(&task->context, &cq_));
        task->response_reader->StartCall();
        task->response_reader->Finish(&task->reply, &task->status, (void*)task);
        return 0;
    }
#endif

/*
  enum NextStatus {
    SHUTDOWN,   ///< The completion queue has been shutdown.
    GOT_EVENT,  ///< Got a new event; \a tag will be filled in with its
                ///< associated value; \a ok indicating its success.
    TIMEOUT     ///< deadline was reached.
  };
*/
    void AsyncCompleteRpc() {
        void *got_tag;
        bool ok = false;
        CompletionQueue::NextStatus nextStatus;
        gpr_timespec time;
        while(running) {
            time.tv_sec = 1;//设置1秒超时
            time.tv_nsec = 0;
            time.clock_type = GPR_TIMESPAN;
            nextStatus = cq_.AsyncNext(&got_tag, &ok, time);
    
            if(nextStatus == CompletionQueue::TIMEOUT)
                continue;
            if(nextStatus == CompletionQueue::SHUTDOWN)
                break;
            //CompletionQueue::GOT_EVENT
            AsyncClientTask *task = static_cast<AsyncClientTask*>(got_tag);
            task->callCb();    
            delete task;
        }
        //while(running && cq_.Next(&got_tag, &ok));
    }    
    void start() {
        running = true;
        thread_ = std::thread(&AsyncClient::AsyncCompleteRpc, this);
    }

    void stop(){
        running = false;
        thread_.join();
    }

    std::string get_server_addr() { return server_addr; }
    
private:
    bool running;
    std::thread thread_;
    std::string server_addr;
    CompletionQueue cq_;
};


template <typename T>
class AsyncService {
public:
    explicit AsyncService(AsyncClient &client):client_(client) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(
            client_.get_server_addr(), grpc::InsecureChannelCredentials());
        stub_ = T::NewStub(channel);
    }

protected:
    AsyncClient &client_;
    std::unique_ptr<typename T::Stub> stub_;
};



#endif
