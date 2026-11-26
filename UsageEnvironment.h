#pragma once

#include"ThreadPool.h"
#include"EventScheduler.h"

class UsageEnvironment{

public:
    static UsageEnvironment* createNew(EventScheduler* scheduler,ThreadPool* hhreadPool);

    UsageEnvironment(EventScheduler* schedule,ThreadPool* threadPool);

    ~UsageEnvironment();

    EventScheduler* scheduler();

    ThreadPool* threadPool();

private:
    EventScheduler* mScheduler;
    ThreadPool* mThreadPool;

};