#include"EventScheduler.h"
#include"EpollPoller.h"
EventScheduler::EventScheduler(){

    mPoller=new EpollPoller();

}
EventScheduler::~EventScheduler(){

    if(mPoller){
        delete mPoller;
        mPoller=nullptr;
    }

}

bool EventScheduler::addIoEvent(IOEvent* event){

    return mPoller->addEvent(event);

}
bool EventScheduler::updateIoEvent(IOEvent* event){

    return mPoller->updateEvent(event);

}
bool EventScheduler::removeIOEvent(IOEvent* event){

    return mPoller->removeEvent(event);

}

void EventScheduler::loop(){

    while(mQuit){
        mPoller->handleEvent(-1);
    }

}