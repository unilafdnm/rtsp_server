#include<TriggerEvent.h>
#include<utility>
TriggerEvent::TriggerEvent(void* arg):mArg(arg)
{
}

void TriggerEvent::setArg(void* arg)
{
    mArg=arg;
}
void TriggerEvent::setTriggerCallback(EventCallback cb){

    mTriggerCallback=cb;

}
void TriggerEvent::handleEvent(){

    if(mTriggerCallback){
        mTriggerCallback(mArg);
    }

}