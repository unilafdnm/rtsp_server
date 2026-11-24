#pragma once

#include<cstdint>

class TimerEvent;
class TimerManager;

class Timer{
public:
    using TimerId=uint32_t;
    using TimeStamp=int64_t;
    using TimeInterval=uint32_t;

    ~Timer();

    static TimeStamp getCurTime();
    static TimeStamp getCurTimestamp();


private:
    friend class TimerManager;
    Timer(TimerEvent* event,TimeStamp mTimestamp,TimeInterval mTimeInterval,TimerId timeId);

    bool handleEvent();

private:
    TimerEvent* mTimeEvent;
    TimeStamp mTimeStamp;
    TimeInterval mTimeInterval;
    TimerId mTimerId;
    bool mRepeat;

};