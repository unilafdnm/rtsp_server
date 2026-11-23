
#include "EpollPoller.h"
#include "io_event.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>

struct TestContext {
    int fd;
    int calls;
};

void onRead(void* arg) {
    auto* ctx = static_cast<TestContext*>(arg);

    uint64_t value = 0;

    if (read(ctx->fd, &value, sizeof(value))
        == sizeof(value)) {
        ++ctx->calls;
        std::printf("read callback: %llu\n",
                    static_cast<unsigned long long>(value));
    }
}

int main() {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        perror("eventfd");
        return 1;
    }

    bool passed = false;

    {
        EpollPoller poller;
        TestContext ctx{fd, 0};
        IOEvent event(fd, &ctx);

        event.enableReadHandling();
        event.setReadCallback(onRead);

        if (poller.addEvent(&event)) {
            uint64_t value = 1;

            if (write(fd, &value, sizeof(value))
                == sizeof(value)) {

                poller.handleEvent(1000);
                passed = (ctx.calls == 1);
            }

            if (!poller.removeEvent(&event)) {
                passed = false;
            }
        }
    }

    close(fd);

    std::puts(passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}
