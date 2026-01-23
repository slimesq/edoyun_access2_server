#include "TcpConnection.h"

TcpConnection::TcpConnection(int _fd) : m_sockio(_fd)
{
}

TcpConnection::~TcpConnection()
{
}

std::string TcpConnection::recive()
{
    char buf[4096]{0};
    this->m_sockio.readline(buf, sizeof(buf));
    return std::string(buf);
}

void TcpConnection::send(std::string const& _msg)
{
    this->m_sockio.writen(_msg.c_str(), _msg.size());
}