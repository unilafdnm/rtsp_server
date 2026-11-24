#pragma once

class TimerEvent{

public:
    using EventCallback=void(*)(void*);

    explicit TimerEvent(void* arg=nullptr);

    void setArg(void* arg);
    void setTimeoutCallback(EventCallback cb);

    bool handleEvent();
    void stop();


private:
    void* mArg{nullptr};
    EventCallback mTimeoutCallback{nullptr};
    bool mIsstop{false};

};