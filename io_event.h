#pragma once

#include<cstdint>
#include<functional>

class IOEvent{

public:
    enum Event : std::uint32_t{
        EVENT_NONE=0,
        EVENT_READ=1u<<0,
        EVENT_WRITE=1u<<1,
        EVENT_ERROR=1u<<2
    };

    using EventCallback=void(*)(void*);

public:
    explicit IOEvent(int fd,void* arg=nullptr);

    int getFd()const noexcept;

    std::uint32_t getEvent()const noexcept;
    std::uint32_t getREvent()const noexcept;

    void* getArg()const noexcept;
    void setArg(void* arg)noexcept;


    void enableReadHandling();
    void enableWriteHandling();

    void disableReadHandling();
    void disableWriteHandling();

    void setREvent(uint32_t events);

    void setReadCallback(EventCallback cb);
    void setWriteCallback(EventCallback cb);
    void setErrorCallback(EventCallback cb);

    void handleEvent();


private:
    int mfd{-1};

    std::uint32_t mEvent{EVENT_NONE};
    std::uint32_t mREvent{EVENT_NONE};

    void* mArg{nullptr};

    EventCallback mReadCallback{nullptr};
    EventCallback mWriteCallback{nullptr};
    EventCallback mErrorCallback{nullptr};

};