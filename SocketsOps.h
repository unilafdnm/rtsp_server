#pragma once

#include<string>
#include<cstdint>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

namespace sockets{

    int createTcpSocket();
    int createUdpSocket();

    bool bind(int sockfd,std::string ip,uint16_t port);
    bool listen(int sockfd,int backlog);
    int accept(int sockfd);

    int write(int sockfd,const void* buf,int size);
    int sendto(int sockfd,const void* buf,int len,const struct sockaddr* destAddr);

    int setNonBlock(int sockfd);
    int setBlock(int sockfd,int writeTimeout);

    void setReuseAddr(int sockfd, int on);
    void setReusePort(int sockfd);
    void setNonBlockAndCloseOnExec(int sockfd);
    void ignoreSigPipeOnSocket(int socketfd);

    void setNoDelay(int sockfd);
    void setKeepAlive(int sockfd);
    void setNoSigpipe(int sockfd);

    void setSendBufSize(int sockfd, int size);
    void setRecvBufSize(int sockfd, int size);

    
    std::string getPeerIp(int sockfd);
    int16_t getPeerPort(int sockfd);

    int getPeerAddr(int sockfd, struct sockaddr_in* addr);

    void close(int sockfd);

    bool connect(int sockfd, std::string ip,
                 uint16_t port, int timeout);

    std::string getLocalIp();


};