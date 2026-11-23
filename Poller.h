#pragma once

#include"io_event.h"
#include<map>

class Poller{
public:
    Poller()=default;
    virtual ~Poller()=default;
    
    Poller(const Poller&)=delete;
    Poller& operator=(const Poller&)=delete;

    virtual bool addEvent(IOEvent* event)=0;
    virtual bool updateEvent(IOEvent* event)=0;
    virtual bool removeEvent(IOEvent* event)=0;
    virtual void handleEvent(int timeoutMs)=0;

protected:
    std::map<int,IOEvent*> mEventMap;
};