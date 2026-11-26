#include"Thread.h"

Thread::~Thread(){

    if (mIsStart && !mIsDetach) {
        detach();
    }

}

bool Thread::start(void *arg){

    mArg=arg;
    mThreadId=std::thread(threadRun,this);

    mIsStart=true;
    return true;
}

bool Thread::detach(){

    if(!mIsStart){
        return false;
    }
    if(mIsDetach){
        return true;
    }

    mThreadId.detach();
    mIsDetach=true;
    return true;

}
bool Thread::join(){

    if(!mIsStart || mIsDetach){
        return false;
    }
    mThreadId.join();
    mIsStart=false;

    return true;

}

Thread::Thread()
    : mArg(nullptr),
      mIsStart(false),
      mIsDetach(false)
{
}



void* Thread::threadRun(void* arg){

    Thread* thread=static_cast<Thread*>(arg);

    thread->run(thread->mArg);

    return nullptr;
}

