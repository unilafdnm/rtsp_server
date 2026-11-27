#include "TcpConnection.h"
#include "SocketsOps.h"

#include <cerrno>
#include <stdexcept>
TcpConnection::TcpConnection(UsageEnvironment* env,int clientFd)
    :mEnv(env),mClientFd(clientFd),mClientIOEvent(nullptr)
    ,mDisConnectCallback(nullptr),mArg(nullptr)
{

    mClientIOEvent=new IOEvent(clientFd,this);
    mClientIOEvent->setReadCallback(readCallback);
    mClientIOEvent->setWriteCallback(writeCallback);
    mClientIOEvent->setErrorCallback(errorCallback);


    mClientIOEvent->enableReadHandling();

    if(!mEnv->scheduler()->addIoEvent(mClientIOEvent)){
        delete mClientIOEvent;
        sockets::close(mClientFd);
        throw std::runtime_error("addIoEvent failed");
    }

}
TcpConnection::~TcpConnection(){

    mEnv->scheduler()->removeIOEvent(mClientIOEvent);

    delete mClientIOEvent;

    sockets::close(mClientFd);

}

void TcpConnection::readCallback(void* arg){

    auto* conn=static_cast<TcpConnection*>(arg);
    conn->handleRead();

}

void TcpConnection::enableReadHandling(){

    if(mClientIOEvent->getEvent() & IOEvent::EVENT_READ){
        return;
    }

    mClientIOEvent->enableReadHandling();
    mEnv->scheduler()->updateIoEvent(mClientIOEvent);

}
void TcpConnection::enableWriteHandling(){

    if(mClientIOEvent->getEvent() & IOEvent::EVENT_WRITE){
        return;
    }

    mClientIOEvent->enableWriteHandling();
    mEnv->scheduler()->updateIoEvent(mClientIOEvent);

}

void TcpConnection::disableReadHandling(){

    if(!(mClientIOEvent->getEvent() & IOEvent::EVENT_READ)){
        return;
    }

    mClientIOEvent->enableReadHandling();
    mEnv->scheduler()->updateIoEvent(mClientIOEvent);

}
void TcpConnection::disableWriteHandling(){

    if(!(mClientIOEvent->getEvent() & IOEvent::EVENT_WRITE)){
        return;
    }

    mClientIOEvent->enableWriteHandling();
    mEnv->scheduler()->updateIoEvent(mClientIOEvent);

}
void TcpConnection::handleRead(){

    int ret=mInputBuffer.read(mClientFd);

    if(ret>0){
        handleReadBytes();
    }else if(ret == 0){

        handleDisConnect();

    }else if(errno == EAGAIN || errno == EWOULDBLOCK ||errno == EINTR){
            return;
    }
    handleDisConnect();
}


void TcpConnection::handleWrite(){

    while(mOutBuffer.readableBytes() > 0){

        int n=::send(mClientFd,mOutBuffer.peek(),mOutBuffer.readableBytes(),MSG_NOSIGNAL);
        if(n > 0){
            mOutBuffer.retrieve(n);
        }else if(n <0 && errno==EINTR){
            continue;
        }else if(n <0 && (errno == EAGAIN || errno ==EWOULDBLOCK)){
            return;
        }else{
            handleError();
            return;
        }


    }

    disableWriteHandling();

}
void TcpConnection::handleError(){

    handleDisConnect();

}





void TcpConnection::handleReadBytes(){

    mInputBuffer.retrieveAll();

}

void TcpConnection::setDisConnectCallback(DisConnectCallback cb, void* arg)
{
    mDisConnectCallback = cb;
    mArg = arg;
}
void TcpConnection::handleDisConnect()
{
    if (mDisConnectCallback) {
        mDisConnectCallback(mArg, mClientFd);
    }
}


void TcpConnection::writeCallback(void* arg){

    auto* conn=static_cast<TcpConnection*>(arg);
    conn->handleWrite();

}
void TcpConnection::errorCallback(void* arg){

    auto* conn=static_cast<TcpConnection*>(arg);
    conn->handleError();

}