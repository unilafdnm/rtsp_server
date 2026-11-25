#pragma once

#include "Timer.h"
#include <cstdint>
#include <map>

class Poller;
class EventScheduler;
class IOEvent;
class TimerEvent;

class TimerManager{

public:
    using TimerId=uint32_t;
    using TimeStamp=int64_t;
    using TimeInterval=uint32_t;


    static TimerManager* createNew(EventScheduler* scheduler);

    TimerManager(EventScheduler* shceduler);
    ~TimerManager();

    TimerId addTimer(TimerEvent* event,TimeStamp timestamp,TimeInterval interval);
    bool removeTimer(TimerId TimerId);

private:
    static void readCallback(void* arg);
    void handleRead();
    void modifyTimeout();

private:
    Poller* mPoller{nullptr};
    std::map<TimerId,Timer> mTimers;
    std::multimap<TimeStamp,Timer> mEvents;
    uint32_t mLastTimerId{0};
    int mTimerFd{-1};
    IOEvent* mTimerIOEvent{nullptr};

};