#include "TimerManager.h"
#include "TimerEvent.h"
#include "EventScheduler.h"
#include "Poller.h"
#include "io_event.h"
#include"Timer.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <cerrno>
#include <stdexcept>
#include <system_error>

TimerManager* TimerManager::createNew(EventScheduler* scheduler){

    if(!scheduler){
        return nullptr;
    }

    return new TimerManager(scheduler);
}

static bool timerFdSetTime(int fd,Timer::TimeStamp when,Timer::TimeInterval period){

    struct itimerspec newVal{};

    newVal.it_value.tv_sec=when/1000;
    newVal.it_value.tv_nsec=(when%1000)*1000000;

    newVal.it_interval.tv_sec=period/1000;
    newVal.it_interval.tv_nsec=(period%1000)*1000000;

    int ret=timerfd_settime(fd,TFD_TIMER_ABSTIME,&newVal,nullptr);
    return ret == 0;

}

TimerManager::TimerManager(EventScheduler* shceduler)
    :mPoller(shceduler->getPoller()),mLastTimerId(0),mTimerFd(-1)
    ,mTimerIOEvent(nullptr)
{
    mTimerFd=timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);

    mTimerIOEvent=new IOEvent(mTimerFd,this);

    mTimerIOEvent->enableReadHandling();

    mTimerIOEvent->setReadCallback(readCallback);

    if(!mPoller->addEvent(mTimerIOEvent)){
        throw std::runtime_error("Register timerfd failed");
    }

}
TimerManager::~TimerManager(){

    if(mTimerIOEvent){
        mPoller->removeEvent(mTimerIOEvent);
        delete mTimerIOEvent;
        mTimerIOEvent=nullptr;
    }
    if(mTimerFd>=0){
        close(mTimerFd);
        mTimerFd=-1;
    }

}

TimerManager::TimerId TimerManager::addTimer(TimerEvent* event,TimeStamp timestamp,TimeInterval interval){

    if(!event || timestamp <= 0){
        return 0;
    }

    if(mLastTimerId == UINT32_MAX){
        return 0;
    }

    TimerId id=++mLastTimerId;


    Timer timer(event,timestamp,interval,mLastTimerId);

    mTimers.insert({mLastTimerId,timer});

    mEvents.insert({timestamp,timer});
    
    modifyTimeout();
    return id;

}
bool TimerManager::removeTimer(TimerId timerId){

    auto it=mTimers.find(timerId);
    if(it == mTimers.end()){
        return false;
    }

    auto timestamp=it->second.mTimeStamp;
    auto range=mEvents.equal_range(timestamp);

    for(auto iter = range.first;iter!=range.second;++iter){
        if(iter->second.mTimerId == timerId){
            mEvents.erase(iter);
            break;
        }
    }

    mTimers.erase(timerId);
    modifyTimeout();

    return true;

}

void TimerManager::readCallback(void* arg){
    
    auto* manager=static_cast<TimerManager*>(arg);

    manager->handleRead();

}

void TimerManager::handleRead(){

    auto now=Timer::getCurTime();

    while(!mEvents.empty()){
        auto it=mEvents.begin();

        if(it->first > now){
            break;
        }
        Timer timer=it->second;
        mEvents.erase(it);
        bool isStop=timer.handleEvent();

        if(timer.mRepeat){
            if(isStop){
                mTimers.erase(timer.mTimerId);
            }else{

                auto timerIt=mTimers.find(timer.mTimerId);
                if(timerIt != mTimers.end()){
                    timer.mTimeStamp=now+timer.mTimeInterval;
                    mEvents.emplace(timer.mTimeStamp,timer);
                    timerIt->second=timer;
                }
            }
        }else{
            mTimers.erase(timer.mTimerId);
        }

    }

    modifyTimeout();
}

void TimerManager::modifyTimeout(){

    struct itimerspec ts{};

    if(!mEvents.empty()){
        Timer timer=mEvents.begin()->second;
        timerFdSetTime(mTimerFd,timer.mTimeStamp,timer.mTimeInterval);
    }
    else{
        timerFdSetTime(mTimerFd,0,0);
    }

}