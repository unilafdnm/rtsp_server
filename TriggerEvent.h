#pragma once

class TriggerEvent{

public:
    using EventCallback=void(*)(void*);

    explicit TriggerEvent(void* arg=nullptr);

    void setArg(void* arg);
    void setTriggerCallback(EventCallback cb);
    void handleEvent();

private:
    void* mArg{nullptr};
    EventCallback mTriggerCallback{nullptr};

};