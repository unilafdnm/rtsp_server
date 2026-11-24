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

bool EventScheduler::addTriggerEvent(TriggerEvent* event){

    if(!event){
        return false;
    }

    mTriggerEvents.push_back(event);
    return true;
}
void EventScheduler::handleTriggerEvent(){

    std::vector<TriggerEvent*> pending;
    pending.swap(mTriggerEvents);

    for (TriggerEvent* event : pending) {
        event->handleEvent();
    }

}


void EventScheduler::loop(){

    while(!mQuit){
        
        handleTriggerEvent();
        mPoller->handleEvent(-1);
    }

}

Poller* EventScheduler::getPoller(){
    return mPoller;
}