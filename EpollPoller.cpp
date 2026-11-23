#include"EpollPoller.h"
#include"io_event.h"

#include <cerrno>
#include <system_error>
#include <unistd.h>
#include <cstdint>
#include<chrono>
EpollPoller::EpollPoller(){

    mEpollFd=epoll_create1(EPOLL_CLOEXEC);
    if(mEpollFd < 0){
        throw std::system_error(errno,std::generic_category(),"epoll_create1");
    }

}
EpollPoller::~EpollPoller(){

    if(mEpollFd >=0 ){
        close(mEpollFd);
    }

}

bool EpollPoller::addEvent(IOEvent* event){

    if(!event||event->getFd()<0){
        return false;
    }

    auto it=mEventMap.find(event->getFd());
    if(it != mEventMap.end()){
        return false; 
    }

    uint32_t epollEvents=event->getEvent();
    int fd=event->getFd();
    uint32_t generation=mNextGeneration;
    epoll_event ev{};
    ev.events=toEpollEvents(epollEvents);
    ev.data.u64=makeToken(fd,generation);

    if(epoll_ctl(mEpollFd,EPOLL_CTL_ADD,fd,&ev) < 0){
        return false;
    }
    mNextGeneration++;

    mGenerations.insert({fd,generation});
    mEventMap.insert({fd,event});
    return true;

}
bool EpollPoller::updateEvent(IOEvent* event){

    if(!event){
        return false;
    }


    int fd=event->getFd();
    if(fd <0 ){
        return false;
    }

    auto it = mEventMap.find(fd);
    if(it == mEventMap.end() || it->second != event){
        return false;
    }

    if(mGenerations.find(fd) == mGenerations.end()){
        return false;
    }

    uint32_t generation=mGenerations[fd];
    uint32_t newEpollEvent=toEpollEvents(event->getEvent());

    epoll_event ev{};
    ev.data.u64=makeToken(fd,generation);
    ev.events=newEpollEvent;

    if(epoll_ctl(mEpollFd,EPOLL_CTL_MOD,fd,&ev)<0){
        return false;
    }


    return true;
}
bool EpollPoller::removeEvent(IOEvent* event){

    if(!event){
        return false;
    }
    int fd=event->getFd();
    if(fd < 0){
        return false;
    }

    auto evit=mEventMap.find(fd);
    auto genit=mGenerations.find(fd);
    if(evit == mEventMap.end() || genit == mGenerations.end() || evit->second != event){
        return false;
    }

    if(epoll_ctl(mEpollFd,EPOLL_CTL_DEL,fd,nullptr) < 0){
        if (errno != EBADF && errno != ENOENT) {
            return false;
        }
    }

    mEventMap.erase(fd);
    mGenerations.erase(fd);
    return true;

}
void EpollPoller::handleEvent(int timeoutMs){

    int ret=epoll_wait(mEpollFd,mReadyEvents.data(),MAX_EVENTS,timeoutMs);

    if(ret < 0){
        if(errno != EINTR){
            perror("epoll_wait");
        }

        return;
    }

    for(int i=0;i<ret;i++){
        epoll_event ev=mReadyEvents[i];
        int fd=static_cast<uint32_t>(ev.data.u64);
        uint32_t generation=static_cast<uint32_t>(ev.data.u64>>32);

        auto evit=mEventMap.find(fd);
        auto genit=mGenerations.find(fd);

        if(genit == mGenerations.end() || genit->second != generation ){
            continue;;
        }
        
        if(evit == mEventMap.end()){
            continue;;
        }

        uint32_t readyEvent=ev.events;
        uint32_t ioEvent=toIOEvents(readyEvent);

        evit->second->setREvent(ioEvent);
        evit->second->handleEvent();

    }

}


uint32_t EpollPoller::toEpollEvents(uint32_t events){
    uint32_t result=0;
    if(events & IOEvent::EVENT_READ){
        result|=EPOLLIN | EPOLLRDHUP;
    }

    if(events & IOEvent::EVENT_WRITE){
        result |= EPOLLOUT;
    }
    return result;

}
uint32_t EpollPoller::toIOEvents(uint32_t events){
    uint32_t result=0;
    if(events & (EPOLLIN | EPOLLRDHUP)){
        result|=IOEvent::EVENT_READ;
    }
    if(events & EPOLLOUT){
        result |=IOEvent::EVENT_WRITE;
    }

    if(events & (EPOLLERR | EPOLLHUP)){
        result |= IOEvent::EVENT_ERROR;
    }

    return result;

}
uint64_t EpollPoller::makeToken(int fd,uint32_t generation){

    return (static_cast<uint64_t>(generation)<<32) | (static_cast<uint64_t>(fd));

}