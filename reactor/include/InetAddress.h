#pragma once
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <utils.h/Noncopyable.h>
#include <string>

class InetAddress : public Noncopyable
{
public:
    InetAddress(std::string const& _ip, unsigned short _port);
    InetAddress(const struct sockaddr_in& _addr);
    ~InetAddress();
    std::string getIp();
    unsigned short getPort();
    struct sockaddr_in* getInetAddressPtr();

private:
    struct sockaddr_in m_addr;
};