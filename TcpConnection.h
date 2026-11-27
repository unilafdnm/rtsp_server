#pragma once

#include"UsageEnvironment.h"
#include"io_event.h"
#include"Buffer.h"


class TcpConnection{

public:
    using DisConnectCallback=void(*)(void*,int);

    TcpConnection(UsageEnvironment* env,int clientFd);
    virtual ~TcpConnection();

    void setDisConnectCallback(DisConnectCallback cb,void* arg);

protected:
    void enableReadHandling();
    void enableWriteHandling();

    void disableReadHandling();
    void disableWriteHandling();

    void handleRead();

    virtual void handleReadBytes();
    virtual void handleWrite();
    virtual void handleError();

    void handleDisConnect();

private:
    static void readCallback(void* arg);
    static void writeCallback(void* arg);
    static void errorCallback(void* arg);


private:
    UsageEnvironment* mEnv;
    int mClientFd;
    IOEvent* mClientIOEvent;

    DisConnectCallback mDisConnectCallback;
    void* mArg;

    Buffer mInputBuffer;
    Buffer mOutBuffer;

    char mBuffer[2048];


};