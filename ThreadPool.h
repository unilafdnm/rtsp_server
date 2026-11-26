#pragma once

#include"Thread.h"

#include<queue>
#include<vector>
#include<mutex>
#include<condition_variable>

class ThreadPool{

public:
    class Task{
        public:
            using TaskCallback=void(*)(void*);
            Task();
            void setTaskCallback(TaskCallback cb,void* arg);
            void handle();
            bool operator=(const Task& task);
        private:
            TaskCallback mTaskCallback;
            void* mArg;
    };

    static ThreadPool* createNew(int num);
    explicit ThreadPool(int num);
    ~ThreadPool();

    void addTask(Task& task);

private:
        void loop();
        class MThread:public Thread{
            protected:
                void run(void* arg)override;
        };

        void createThreads();
        void canceThreads();

private:
    std::queue<Task> mTaskQueue;

    std::mutex mMtx;
    std::condition_variable mCon;

    std::vector<MThread> mThreads;

    bool mQuit;

};