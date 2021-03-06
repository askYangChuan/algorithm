#ifndef __GRPC_CLIENT_H_
#define __GRPC_CLIENT_H_

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

class AsyncClientTask;

//std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::helloworld::HelloReply>> 
//PrepareAsyncSayHello(::grpc::ClientContext* context, const ::helloworld::HelloRequest& request, ::grpc::CompletionQueue* cq) 

template<typename K, typename T>
using PrepareFunc = boost::function<std::unique_ptr< ClientAsyncResponseReader< T>> (ClientContext*,const K&,CompletionQueue*)>;


//using PrepareFunc = boost::function<void* (ClientContext*,T&,CompletionQueue*)>;
//typedef boost::function<void* (void)> PrepareFunc;
//typedef boost::function<void* (ClientContext*, CompletionQueue*)> PrepareFunc;
typedef boost::function<void (AsyncClientTask*)> RpcCbFunc;
typedef void (*RpcCb)(AsyncClientTask*);




class AsyncClientTask {
public:
    void setCb(RpcCb cb) {
        cb_ = cb;
    }

    void setCb(RpcCbFunc cb) {
        fb_ = cb;
    }

    void callCb() {
        if(cb_ != NULL){
            cb_(this);
        }
        if(!fb_.empty()) {
            fb_(this);
        }
    }
protected:
    RpcCbFunc fb_;
    RpcCb cb_;
};

template <typename T>
class AsyncClientTaskT: public AsyncClientTask {
public:
    T reply;
    ClientContext context;
    Status status;
    std::unique_ptr<ClientAsyncResponseReader<T> > response_reader;
};


template <typename T>
class ClientTask {
public:
    ClientTask(AsyncClientTask *task):task_(static_cast<AsyncClientTaskT<T> *>(task)){}

    T &getReply() {
        return task_->reply;
    }

    ClientContext &getClientContext() {
        return task_->context;
    }

    Status &getStatus() {
        return task_->status;
    }

private:
    AsyncClientTaskT<T> *task_;
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
    int doRpc(const K &req, PrepareFunc<K, T> pf, RpcCb cb = NULL) {
        AsyncClientTaskT<T> *task = new AsyncClientTaskT<T>();
        //call->response_reader.reset(static_cast<ClientAsyncResponseReader<T>*>(pf()));
        task->setCb(cb);
        //task->response_reader = static_cast<ClientAsyncResponseReader<T>*>(pf(&task->context, req, &cq_));
        task->response_reader = pf(&task->context, req, &cq_);
        task->response_reader->StartCall();
        task->response_reader->Finish(&task->reply, &task->status, (void*)task);
        return 0;
    }

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
            
            if(nextStatus == CompletionQueue::SHUTDOWN)
                break;

            if(!ok) {
                //ok为false，自测出来一种情况
                //服务器收到请求后，不执行finish，表明收到了，但是没应答。
                //如果出现ok为false的情况，应该提示一下。这种表明服务器忙之类的。
                printf("AsyncNext ok failed, Server maybe busy\n");
                continue;
            }
            if(nextStatus == CompletionQueue::TIMEOUT)
                continue;
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
        //cq_.Shutdown();
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

#define SetPrepareFunc(Service, fb) boost::bind(&Service::Stub::fb, stub_.get(), _1, _2, _3)
#define SetCbFuncN(fb, args...) boost::bind(&fb, _1, ##args)
#define SetClassCbFuncN(fb, this, args...) boost::bind(&fb, this, _1, ##args)

//#define GetPrepareFunc(fb) boost::bind(&T::Stub::fb, stub_.get(), _1, _2, _3)

    


protected:
    AsyncClient &client_;
    std::unique_ptr<typename T::Stub> stub_;
};



//sync client
class SyncClient {
public:
    explicit SyncClient(std::string addr):server_addr(addr) {}
    
    std::string get_server_addr() { return server_addr; }
    
private:
    std::string server_addr;
};

template<typename K, typename T>
using doRpcFunc = boost::function<Status (ClientContext*, const K&, T*)>;

template<typename T>
class SyncClientTask {
public:
    T &getReply() { return reply;}
    ClientContext &getClientContext() { return context; }
    Status &getStatus() { return status; }
    void setStatus(Status status) { this->status = status;}
private:
    T reply;
    ClientContext context;
    Status status;
};

template <typename T>
class SyncService {
public:
    explicit SyncService(SyncClient &client):client_(client) {
        std::shared_ptr<Channel> channel = grpc::CreateChannel(
            client_.get_server_addr(), grpc::InsecureChannelCredentials());
        stub_ = T::NewStub(channel);
    }


#define SetSyncRpcFunc(Service, fb) boost::bind(&Service::Stub::fb, stub_.get(), _1, _2, _3)

    template<typename K, typename N>
    SyncClientTask<N> *doRpc(const K &req, doRpcFunc<K,N> fb) {
        SyncClientTask<N> *task = new SyncClientTask<N>();
        Status status = fb(&task->getClientContext(), req, &task->getReply());
        task->setStatus(status);
        return task;
    }


protected:
    SyncClient &client_;
    std::unique_ptr<typename T::Stub> stub_;
};


#endif
