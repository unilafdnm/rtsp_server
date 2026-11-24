#include"Timer.h"
#include"TimerEvent.h"
#include<chrono>

Timer::~Timer(){

}

Timer::TimeStamp Timer::getCurTime(){

    struct timespec ts{};

    clock_gettime(CLOCK_MONOTONIC,&ts);

    TimeStamp ms=(int64_t)ts.tv_nsec/1000000+ts.tv_sec*1000;
    return ms;

}
Timer::TimeStamp Timer::getCurTimestamp(){

    auto tp=std::chrono::system_clock::now();

    auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
    TimeStamp timestampMs=ms.count();
    return timestampMs;
}



Timer::Timer(TimerEvent* event,TimeStamp timestamp,TimeInterval timeInterval,TimerId timeId)
    :mTimeEvent(event),mTimeStamp(timestamp),mTimeInterval(timeInterval),mTimerId(timeId)
{

    mRepeat=(mTimeInterval>0);
}

bool Timer::handleEvent(){

    if(!mTimeEvent){
        return false;
    }

    return mTimeEvent->handleEvent();


}