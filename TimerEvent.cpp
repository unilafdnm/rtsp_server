#include"TimerEvent.h"


TimerEvent::TimerEvent(void* arg):mArg(arg)
{

}

void TimerEvent::setArg(void* arg){

    mArg=arg;

}
void TimerEvent::setTimeoutCallback(EventCallback cb){

    mTimeoutCallback=cb;

}

bool TimerEvent::handleEvent(){

    if(mIsstop){
        return true;
    }

    if(mTimeoutCallback){
       mTimeoutCallback(mArg); 
    }

    return mIsstop;

}
void TimerEvent::stop(){
    mIsstop=true;
}