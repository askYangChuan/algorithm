��grpc��C++�̳̼򵥷�װ��û�д�����ʽrpc��

client��Ҫע��ĵط�
1����Ҫʵ��һ���̳�service���࣬��
ͬ��Ϊ
class GreeterImpl2 : public SyncService<Greeter> {}
�첽Ϊ
class GreeterImpl : public AsyncService<Greeter> {}

2������ͬ��rpc��ʱ�򣬵��÷�ʽΪ
    SyncClientTask<HelloReply> *task = doRpc<HelloRequest, HelloReply>(req, SetSyncRpcFunc(Greeter, SayHello));
    ����task->getStatus().ok()�õ����ؽ��

3�������첽rpc��ʱ�������ֵ��÷�ʽ.
        //client_.doRpc<HelloRequest, HelloReply>(req, SetPrepareFunc(Greeter, PrepareAsyncSayHello), local_getHello);
        client_.doRpc<HelloRequest, HelloReply>(req, SetPrepareFunc(Greeter, PrepareAsyncSayHello), SetCbFuncN(local_getHello, name));
        //client_.doRpc<HelloRequest, HelloReply>(req, SetPrepareFunc(Greeter, PrepareAsyncSayHello), SetClassCbFuncN(GreeterImpl::getHello, this));
    ��һ�֣����ûص�����Ϊlocal_hello������local_hello�����Ĳ���Ϊvoid local_getHello(AsyncClientTask *task);
    �ڶ��֣����ûص�������SetCbFuncN�����ã�local_getHello����������Ϊvoid local_getHello(AsyncClientTask *task, std::string name);
    �����֣����ûص�������SetClassCbFuncN�����ã�GreeterImpl::getHello�ĺ�������Ϊvoid getHello(AsyncClientTask *task);

4��ClientTask<HelloReply> task
    ClientTask���ڷ�װһ��Ӧ�����ݶ����ڸ�Ӧ��֮�С�
    

5��cq_.AsyncNext(&got_tag, &ok, time);
    //�ͻ��˵�okΪfalse���Բ����һ�����
    //�������յ�����󣬲�ִ��finish�������յ��ˣ�����û����һֱ���С�
    //�������okΪfalse�������Ӧ����ʾһ�¡����ֱ���������æ֮��ġ�
    //��ʱtime��ʱ�ˣ�okҲ��1�����������������
   
   
    
    