#include"InetAddress.h"

Ipv4Address::Ipv4Address(){

}

Ipv4Address::Ipv4Address(std::string ip, uint16_t port)
    :mIp(ip),mPort(port)
{
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &mAddr.sin_addr);
}

void Ipv4Address::setAddr(std::string ip, uint16_t port){

    mIp=ip;
    mPort=port;

    mAddr.sin_family=AF_INET;
    mAddr.sin_port=htons(port);
    inet_pton(AF_INET,ip.data(),&mAddr.sin_addr);

}

std::string Ipv4Address::getIp(){
    return mIp;
}

uint16_t Ipv4Address::getPort(){
    return mPort;
}

struct sockaddr* Ipv4Address::getAddr(){
    return reinterpret_cast<struct sockaddr*>(&mAddr);
}