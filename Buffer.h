#pragma once

#include<cstdlib>
#include<algorithm>
#include<cstdint>
#include<cassert>


class Buffer{

public:
    static const int initialiSize;

    explicit Buffer();
    ~Buffer();

    int readableBytes()const;
    int writeableBytes()const;
    int prependableBytes()const;

    char* peek();
    const char* peek()const;

    const char* findCRLF()const;
    const char* findCRLF(const char* start)const;
    const char* findLastCrlf()const;

    void retrieveReadZero();
    void retrieve(int len);
    void retrieveUntil(const char* end);
    void retrieveAll();

    char* beginWrite();
    const char* beginWrite()const;

    void unwrite(int len);
    void ensureWriteableBytes(int len);
    void makeSpace(int len);

    void append(const char* data,int len);
    void append(const void* data,int len);

    int read(int fd);
    int write(int fd);

private:
    char* begin();
    const char* begin()const;



private:
    char* mBuffer;
    int mBufferSize;
    int mReadIndex;
    int mWriteIndexl;

    static const char* KCRLF;

};