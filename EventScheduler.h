#pragma once

#include"Poller.h"
#include"TriggerEvent.h"
#include<vector>
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


private:
    bool mQuit{false};
    Poller* mPoller{nullptr};

    std::vector<TriggerEvent*> mTriggerEvents;


};