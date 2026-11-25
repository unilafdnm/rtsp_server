#include"EventScheduler.h"
#include"TimerManager.h"
#include"EpollPoller.h"
EventScheduler::EventScheduler(){

    mPoller=new EpollPoller();
    mTimerManager=TimerManager::createNew(this);
}
EventScheduler::~EventScheduler(){

    if(mTimerManager){
        delete mTimerManager;
        mTimerManager=nullptr;
    }

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

Timer::TimerId EventScheduler::addTimedEventRunAfter(TimerEvent* event,Timer::TimeInterval delay){

    auto timestamp=Timer::getCurTime();
    timestamp+=delay;

    return mTimerManager->addTimer(event,timestamp,0);

}

Timer::TimerId EventScheduler::addTimedEventRunAt(TimerEvent* event,Timer::TimeStamp when){

    return mTimerManager->addTimer(event,when,0);


}

Timer::TimerId EventScheduler::addTimedEventRunEvery(TimerEvent* event,Timer::TimeInterval interval){

    auto timestamp=Timer::getCurTime()+interval;
    
    return mTimerManager->addTimer(event,timestamp,interval);

}

bool EventScheduler::removeTimedVent(Timer::TimerId timerId){

    return mTimerManager->removeTimer(timerId);

}