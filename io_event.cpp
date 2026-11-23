#include"io_event.h"

#include<sys/epoll.h>
#include<stdexcept>
#include<utility>

IOEvent::IOEvent(int fd,void* arg):mfd(fd),mArg(arg)
{
    if(mfd<0){
        throw std::invalid_argument("IOEvent:fd must be >= 0");
    }
}

int IOEvent::getFd()const noexcept{
    return mfd;
}

std::uint32_t IOEvent::getEvent()const noexcept{
    return mEvent;
}
std::uint32_t IOEvent::getREvent()const noexcept{
    return mREvent;
}

void* IOEvent::getArg()const noexcept{
    return mArg;
}
void IOEvent::setArg(void* arg)noexcept{
    mArg=arg;
}


void IOEvent::enableReadHandling(){
    mEvent|=EVENT_READ;
}
void IOEvent::enableWriteHandling(){
    mEvent|=EVENT_WRITE;
}

void IOEvent::disableReadHandling(){
    mEvent&=~EVENT_READ;
}
void IOEvent::disableWriteHandling(){
    mEvent&=~EVENT_WRITE;
}

void IOEvent::setREvent(uint32_t events){
    mREvent=events;
}

void IOEvent::setReadCallback(EventCallback cb){
    mReadCallback=cb;
}
void IOEvent::setWriteCallback(EventCallback cb){
    mWriteCallback=cb;
}
void IOEvent::setErrorCallback(EventCallback cb){
    mErrorCallback=cb;
}

void IOEvent::handleEvent(){

    if((mREvent & EVENT_READ)!=0){
        if(mReadCallback != nullptr){
            mReadCallback(mArg);
        }
    }

    if((mREvent & EVENT_WRITE)!=0){
        if(mWriteCallback != nullptr){
            mWriteCallback(mArg);
        }
    }

    if((mREvent & EVENT_ERROR) != 0){
        if(mErrorCallback != nullptr){
            mErrorCallback(mArg);
        }
    }
}