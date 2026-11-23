#pragma once

#include"Poller.h"

class EventScheduler{

public:
    EventScheduler();
    ~EventScheduler();

    bool addIoEvent(IOEvent* event);
    bool updateIoEvent(IOEvent* event);
    bool removeIOEvent(IOEvent* event);

    void loop();


private:
    bool mQuit{false};
    Poller* mPoller{nullptr};
};