#include "EventScheduler.h"
#include "TimerEvent.h"
#include "TriggerEvent.h"
#include "io_event.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>

void check(bool result, const char* name)
{
    printf("[%s] %s\n",
           result ? "PASS" : "FAIL", name);

    if (!result) {
        std::exit(EXIT_FAILURE);
    }
}

void countCallback(void* arg)
{
    auto* count = static_cast<int*>(arg);
    ++(*count);
}

// IOEvent 测试
struct IOContext {
    int fd;
    int count{0};
};

void readCallback(void* arg)
{
    auto* ctx = static_cast<IOContext*>(arg);

    uint64_t value = 0;

    if (read(ctx->fd, &value, sizeof(value))
        == sizeof(value)) {
        ++ctx->count;
    }
}

// 周期定时器停止测试
struct RepeatContext {
    TimerEvent* event{nullptr};
    int count{0};
};

void repeatCallback(void* arg)
{
    auto* ctx = static_cast<RepeatContext*>(arg);

    ++ctx->count;

    if (ctx->count == 3) {
        ctx->event->stop();
    }
}

int main()
{
    EventScheduler scheduler;

    // 1. IOEvent
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    check(fd >= 0, "create eventfd");

    IOContext ioCtx{fd};

    IOEvent ioEvent(fd, &ioCtx);

    ioEvent.enableReadHandling();
    ioEvent.setReadCallback(readCallback);

    check(scheduler.addIoEvent(&ioEvent),
          "register IOEvent");

    uint64_t value = 1;

    check(write(fd, &value, sizeof(value))
          == sizeof(value), "write eventfd");

    // 2. TriggerEvent
    int triggerCount = 0;

    TriggerEvent trigger(&triggerCount);
    trigger.setTriggerCallback(countCallback);

    scheduler.addTriggerEvent(&trigger);

    // 3. 一次性延迟任务
    int afterCount = 0;

    TimerEvent afterEvent(&afterCount);
    afterEvent.setTimeoutCallback(countCallback);

    check(scheduler.addTimedEventRunAfter(
              &afterEvent, 50) != 0,
          "register RunAfter");

    // 4. 两个任务同时到期
    int atCount1 = 0;
    int atCount2 = 0;

    TimerEvent atEvent1(&atCount1);
    TimerEvent atEvent2(&atCount2);

    atEvent1.setTimeoutCallback(countCallback);
    atEvent2.setTimeoutCallback(countCallback);

    auto when = Timer::getCurTime() + 100;

    check(scheduler.addTimedEventRunAt(
              &atEvent1, when) != 0,
          "register RunAt 1");

    check(scheduler.addTimedEventRunAt(
              &atEvent2, when) != 0,
          "register RunAt 2");

    // 5. 周期任务执行三次后停止
    RepeatContext repeatCtx;

    TimerEvent repeatEvent(&repeatCtx);
    repeatCtx.event = &repeatEvent;

    repeatEvent.setTimeoutCallback(repeatCallback);

    check(scheduler.addTimedEventRunEvery(
              &repeatEvent, 40) != 0,
          "register RunEvery");

    // 6. 提前取消任务
    int cancelCount = 0;

    TimerEvent cancelEvent(&cancelCount);
    cancelEvent.setTimeoutCallback(countCallback);

    auto cancelId = scheduler.addTimedEventRunAt(
        &cancelEvent, when);

    check(cancelId != 0, "register cancel timer");

    check(scheduler.removeTimedVent(cancelId),
          "cancel timer before expiration");

    // 7. 周期任务执行一次后取消
    int cancelRepeatCount = 0;

    TimerEvent cancelRepeatEvent(&cancelRepeatCount);

    cancelRepeatEvent.setTimeoutCallback(
        countCallback);

    auto repeatId = scheduler.addTimedEventRunEvery(
        &cancelRepeatEvent, 30);

    check(repeatId != 0,
          "register cancel repeat timer");

    bool removed = false;

    // 统一驱动事件调度
    auto start = Timer::getCurTime();

    while (Timer::getCurTime() - start < 600) {

        scheduler.handleTriggerEvent();

        scheduler.getPoller()->handleEvent(20);

        if (!removed && cancelRepeatCount >= 1) {
            removed = scheduler.removeTimedVent(
                repeatId);
        }
    }

    // 验证结果
    check(ioCtx.count == 1,
          "IOEvent");

    check(triggerCount == 1,
          "TriggerEvent");

    check(afterCount == 1,
          "RunAfter");

    check(atCount1 == 1,
          "RunAt 1");

    check(atCount2 == 1,
          "RunAt 2");

    check(repeatCtx.count == 3,
          "RunEvery and stop");

    check(cancelCount == 0,
          "cancel before expiration");

    check(removed && cancelRepeatCount == 1,
          "cancel after rescheduling");

    check(scheduler.removeIOEvent(&ioEvent),
          "remove IOEvent");

    close(fd);

    printf("\nALL TESTS PASSED\n");

    return 0;
}