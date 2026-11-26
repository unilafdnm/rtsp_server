#include"SocketsOps.h"

#include<fcntl.h>
#include<unistd.h>
#include<netinet/tcp.h>
#include<sys/time.h>
#include<sys/select.h>
#include<cerrno>
#include<cstdio>

int sockets::createTcpSocket(){

    int sockfd=socket(AF_INET,SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK,IPPROTO_TCP);
    return sockfd;
}

int sockets::createUdpSocket(){

    return ::socket(AF_INET,SOCK_DGRAM|SOCK_CLOEXEC|SOCK_NONBLOCK,IPPROTO_UDP);
    
}

bool sockets::bind(int sockfd,std::string ip,uint16_t port){

    struct sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_port=htons(port); 
    int ret=inet_pton(AF_INET,ip.data(),&addr.sin_addr);
    if(ret<=0){
        return false;
    }

    ret=::bind(sockfd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));

    if(ret <0){
        return false;
    }
    return true;
}
bool sockets::listen(int sockfd,int backlog){

    if(::listen(sockfd,backlog)<0){
        return false;
    }
    return true;

}
int sockets::accept(int sockfd){

    struct sockaddr_in addr{};
    socklen_t len=sizeof(addr);

    int fd=::accept(sockfd,reinterpret_cast<sockaddr*>(&addr),&len);

    if(fd<0){
        return -1;
    }

    setNonBlockAndCloseOnExec(fd);

    return fd;

}

int sockets::write(int sockfd,const void* buf,int size){

    return ::write(sockfd,buf,size);

}
int sockets::sendto(int sockfd,const void* buf,int len,const struct sockaddr* destAddr){

    return ::sendto(sockfd,buf,len,0,destAddr,sizeof(struct sockaddr_in));
}



int sockets::setNonBlock(int sockfd){

    int flags=::fcntl(sockfd,F_GETFL,0);

    if(flags < 0){
        return -1;
    }

    return ::fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);


}
int sockets::setBlock(int sockfd,int writeTimeout){

    int flags=::fcntl(sockfd,F_GETFL,0);
    if(flags < 0){
        return -1;
    }

    flags=flags& ~O_NONBLOCK;

    struct timeval tv{};
    tv.tv_sec=writeTimeout/1000;
    tv.tv_usec=(writeTimeout%1000)*1000;

    int ret=setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&tv,sizeof(tv));
    if(ret<0){
        return -1;
    }

    return ::fcntl(sockfd,F_SETFL,flags);

}

void sockets::setNonBlockAndCloseOnExec(int sockfd){

    int  flags=::fcntl(sockfd,F_GETFL,0);

    if(flags<0){
        perror("FL flags get failed");
        return ;
    }
    flags|=O_NONBLOCK;
    if(::fcntl(sockfd,F_SETFL,flags)<0){
        perror("FL flags set failed");
        return ;
    }

    flags=::fcntl(sockfd,F_GETFD,0);
    if(flags<0){
        perror("FD flags get failed");
        return ;
    }

    flags|=FD_CLOEXEC;

    if(::fcntl(sockfd,F_SETFD,flags) < 0){
        perror("FD flags set failed");
        return ;
    }

    return ;

}

void sockets::setReuseAddr(int sockfd, int on){

    int optval=on?1:0;

    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));


}
void sockets::setReusePort(int sockfd){

    int optval=1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));

}

void sockets::setNoDelay(int sockfd){

    int optval=1;
    setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof(optval));

}
void sockets::setKeepAlive(int sockfd){

    int optval=1;
    setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));

}
void sockets::setNoSigpipe(int sockfd);

void sockets::setSendBufSize(int sockfd, int size){

    int optval=size;
    setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,&optval,sizeof(optval));

}
void sockets::setRecvBufSize(int sockfd, int size){

    int optval=size;
    setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&optval,sizeof(optval));
}



std::string sockets::getPeerIp(int sockfd){


    struct sockaddr_in addr{};
    int ret=getPeerAddr(sockfd,&addr);
    if(ret<0){
        return "0.0.0.0";
    }

    char buf[1024]={0};

    const char* p=inet_ntop(AF_INET,&addr.sin_addr,buf,INET_ADDRSTRLEN);
    if(p == nullptr){
        return "0.0.0.0";
    }
    return std::string(buf);

}
int16_t sockets::getPeerPort(int sockfd){

    struct sockaddr_in addr{};
    int ret=getPeerAddr(sockfd,&addr);
    if(ret<0){
        return 0;
    }
    return ntohs(addr.sin_port);

}

int sockets::getPeerAddr(int sockfd, struct sockaddr_in* addr){

    socklen_t addrlen=sizeof(struct sockaddr_in);

    return ::getpeername(sockfd,reinterpret_cast<struct sockaddr*>(addr),&addrlen);
}

void sockets::close(int sockfd){
    ::close(sockfd);
}
