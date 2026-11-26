#include"ThreadPool.h"

ThreadPool::Task::Task():mTaskCallback(nullptr),mArg(nullptr)
{

}
void ThreadPool::Task::setTaskCallback(TaskCallback cb,void* arg){
    mTaskCallback=std::move(cb);
    mArg=arg;
}
void ThreadPool::Task::handle(){

    if(mTaskCallback){
        mTaskCallback(mArg);
    }

}
bool ThreadPool::Task::operator=(const Task& task){

    this->mTaskCallback=task.mTaskCallback;
    this->mArg=task.mArg;
    return true;

}

void ThreadPool::MThread::run(void* arg){

    ThreadPool* pool=static_cast<ThreadPool*>(arg);
    pool->loop();

}

ThreadPool::ThreadPool(int num):mThreads(num),mQuit(false)
{
    createThreads();
}

void ThreadPool::createThreads(){

    std::unique_lock<std::mutex> lock(mMtx);

    for(auto& thread:mThreads){
        thread.start(this);
    }

}

void ThreadPool::canceThreads(){

    std::unique_lock<std::mutex> lock(mMtx);

    for(auto& thread:mThreads){
        thread.detach();
    }

}


ThreadPool* ThreadPool::createNew(int num){

    return new ThreadPool(num);

}

ThreadPool::~ThreadPool(){
    canceThreads();
}

void ThreadPool::addTask(ThreadPool::Task& task){
    std::unique_lock<std::mutex> mMtx;

    mTaskQueue.push(std::move(task));

    mCon.notify_one();
}


void ThreadPool::loop(){

    while(true){
        std::unique_lock<std::mutex> lock(mMtx);

        mCon.wait(lock,[this](){
            return !mTaskQueue.empty() || mQuit;
        });

        if(mQuit){
            canceThreads();
            return;
        }

        Task task=mTaskQueue.front();
        mTaskQueue.pop();
        lock.unlock();
        task.handle();
    }
}