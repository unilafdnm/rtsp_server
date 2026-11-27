#include"UsageEnvironment.h"

UsageEnvironment* UsageEnvironment::createNew(EventScheduler* scheduler,ThreadPool* threadPool){

    return new UsageEnvironment(scheduler,threadPool);

}

UsageEnvironment::UsageEnvironment(EventScheduler* schedule,ThreadPool* threadPool)
    :mScheduler(schedule),mThreadPool(threadPool)
{

}

UsageEnvironment::~UsageEnvironment(){


}

EventScheduler* UsageEnvironment::scheduler(){
    return mScheduler;
}

ThreadPool* UsageEnvironment::threadPool(){
    return mThreadPool;
}