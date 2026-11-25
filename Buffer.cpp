#include"Buffer.h"

#include<sys/socket.h>
#include<unistd.h>

Buffer::Buffer()
    :mBufferSize(initialiSize),mReadIndex(0),mWriteIndexl(0)
{
    mBuffer=static_cast<char*>(malloc(mBufferSize));
}
Buffer::~Buffer(){

    free(mBuffer);

}

int Buffer::readableBytes()const{
    return mWriteIndexl-mReadIndex;
}
int Buffer::writeableBytes()const{
    return mBufferSize-mWriteIndexl;
}
int Buffer::prependableBytes()const{
    return mReadIndex;
}

char* Buffer::peek(){
    return begin()+mReadIndex;
}
const char* Buffer::peek()const{
    return begin()+mReadIndex;
}

const char* Buffer::findCRLF()const{

    auto result=std::search(peek(),beginWrite(),KCRLF,KCRLF+2);

    return result == beginWrite() ? nullptr : result;

}
const char* Buffer::findCRLF(const char* start)const{

    auto result=std::search(start,beginWrite(),KCRLF,KCRLF+2);

    return result == beginWrite() ? nullptr : result;
}
const char* Buffer::findLastCrlf()const{

    auto result=std::search(peek(),beginWrite(),KCRLF,KCRLF+2);

    return result == beginWrite() ? nullptr : result;
}



void Buffer::retrieveReadZero(){

    mReadIndex=0;

}
void Buffer::retrieve(int len){

    if(len<readableBytes()){
        mReadIndex+=len;
    }else{
        retrieveAll();
    }

}
void Buffer::retrieveUntil(const char* end){

    retrieve(static_cast<int>(end -peek()));

}
void Buffer::retrieveAll(){

    mReadIndex=0;
    mWriteIndexl=0;

}

char* Buffer::beginWrite(){
    return begin()+mWriteIndexl;
}
const char* Buffer::beginWrite()const{
    return begin()+mWriteIndexl;
}

void Buffer::unwrite(int len){
    mWriteIndexl+=len;
}
void Buffer::ensureWriteableBytes(int len){
    while(writeableBytes() < len){
        makeSpace(len);
    }
}
void Buffer::makeSpace(int len){

    if(prependableBytes() + writeableBytes() < len){
        mBufferSize=2*mWriteIndexl;
        mBuffer=static_cast<char*>(realloc(mBuffer,mBufferSize));
    }{
        int readSz=readableBytes();
        std::copy(begin()+mReadIndex,begin()+mWriteIndexl,begin());
        mReadIndex=0;
        mWriteIndexl=readSz;
    }

}

void Buffer::append(const char* data,int len){

    ensureWriteableBytes(len);

    std::copy(data,data+len,beginWrite());

    mWriteIndexl+=len;
}
void Buffer::append(const void* data,int len){

    ensureWriteableBytes(len);

    std::copy(data,data+len,beginWrite());

    mWriteIndexl+=len;

}


char* Buffer::begin(){
    return mBuffer;
}
const char* Buffer::begin()const{
    return mBuffer;
}


int Buffer::read(int fd){

    char extrabuf[65536];

    int freeSpace=writeableBytes();

    ssize_t n=::recv(fd,extrabuf,sizeof(extrabuf),0);

    if(n <=0){
        return static_cast<int>(n);
    }

    if(n <= freeSpace){
        std::copy(extrabuf,extrabuf+n,beginWrite());
        mWriteIndexl+=n;
    }
    else{
        std::copy(extrabuf,extrabuf+freeSpace,beginWrite());
        mWriteIndexl+=freeSpace;
        append(extrabuf+freeSpace,static_cast<int>(n)-freeSpace);
    }
    return static_cast<int>(n);


}
int Buffer::write(int fd){
    return static_cast<int>(::write(fd,peek(),readableBytes()));
}