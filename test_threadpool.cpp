#include "ThreadPool.h"

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

void check(bool ok, const char* name)
{
    printf("[%s] %s\n", ok ? "PASS" : "FAIL", name);

    if (!ok) {
        std::exit(EXIT_FAILURE);
    }
}

struct Context {
    std::mutex mutex;
    std::condition_variable cv;

    int started{0};
    int finished{0};
    bool release{false};
};

void countTask(void* arg)
{
    auto* ctx = static_cast<Context*>(arg);

    std::lock_guard<std::mutex> lock(ctx->mutex);

    ++ctx->finished;

    ctx->cv.notify_all();
}

void gatedTask(void* arg)
{
    auto* ctx = static_cast<Context*>(arg);

    std::unique_lock<std::mutex> lock(ctx->mutex);

    ++ctx->started;

    ctx->cv.notify_all();

    ctx->cv.wait(lock, [ctx] {
        return ctx->release;
    });

    lock.unlock();

    std::this_thread::sleep_for(50ms);

    countTask(ctx);
}

int main()
{
    // 1. 两个生产者提交100个任务
    {
        Context ctx;
        ThreadPool pool(4);

        auto producer = [&] {
            ThreadPool::Task task;

            task.setTaskCallback(countTask, &ctx);

            for (int i = 0; i < 50; ++i) {
                pool.addTask(task);
            }
        };

        std::thread p1(producer);
        std::thread p2(producer);

        p1.join();
        p2.join();

        std::unique_lock<std::mutex> lock(ctx.mutex);

        check(ctx.cv.wait_for(lock, 2s, [&] {
            return ctx.finished == 100;
        }), "100 tasks from two producers");
    }

    // 2. 并行执行与析构测试
    {
        Context ctx;
        bool parallel = false;

        {
            ThreadPool pool(2);

            ThreadPool::Task task;

            task.setTaskCallback(gatedTask, &ctx);

            pool.addTask(task);
            pool.addTask(task);

            {
                std::unique_lock<std::mutex> lock(ctx.mutex);

                parallel = ctx.cv.wait_for(lock, 2s, [&] {
                    return ctx.started == 2;
                });

                ctx.release = true;
            }

            ctx.cv.notify_all();

            // 离开作用域，立即析构线程池
        }

        check(parallel,
              "two workers run concurrently");

        {
            std::lock_guard<std::mutex> lock(ctx.mutex);

            check(ctx.finished == 2,
                  "destructor waits for running tasks");
        }
    }

    // 3. 没有任务时销毁线程池
    {
        ThreadPool pool(2);

        std::this_thread::sleep_for(20ms);
    }

    check(true, "idle pool shutdown");

    printf("\nALL TESTS PASSED\n");

    return 0;
}