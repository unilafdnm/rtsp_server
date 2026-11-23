#pragma once

#include"Poller.h"

#include<array>
#include<cstdint>
#include<unordered_map>
#include<sys/epoll.h>

class EpollPoller final : public Poller{

public:
    EpollPoller();
    ~EpollPoller() override;

    bool addEvent(IOEvent* event) override;
    bool updateEvent(IOEvent* event) override;
    bool removeEvent(IOEvent* event) override;
    void handleEvent(int timeoutMs) override;


private:
    static constexpr int MAX_EVENTS=64;
    static uint32_t toEpollEvents(uint32_t events);
    static uint32_t toIOEvents(uint32_t events);
    static uint64_t makeToken(int fd,uint32_t generation);

private:
    int mEpollFd=-1;
    std::array<epoll_event,MAX_EVENTS> mReadyEvents{};
    std::unordered_map<int,uint32_t> mGenerations;
    uint32_t mNextGeneration=1;

};