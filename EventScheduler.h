#pragma once

#include"Poller.h"
#include"TriggerEvent.h"
#include"Timer.h"
#include<vector>

class TimerManager;

class EventScheduler{

public:
    EventScheduler();
    ~EventScheduler();

    bool addIoEvent(IOEvent* event);
    bool updateIoEvent(IOEvent* event);
    bool removeIOEvent(IOEvent* event);

    void loop();

    bool addTriggerEvent(TriggerEvent* event);
    void handleTriggerEvent();
    Poller* getPoller();


    //定时任务接口

    Timer::TimerId addTimedEventRunAfter(TimerEvent* event,Timer::TimeInterval delay);
    Timer::TimerId addTimedEventRunAt(TimerEvent* event,Timer::TimeStamp when);

    Timer::TimerId addTimedEventRunEvery(TimerEvent* event,Timer::TimeInterval interval);

    bool removeTimedVent(Timer::TimerId timerId);


private:
    bool mQuit{false};
    Poller* mPoller{nullptr};

    std::vector<TriggerEvent*> mTriggerEvents;

    TimerManager* mTimerManager{nullptr};
};