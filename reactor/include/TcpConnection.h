#pragma once
#include <string>
#include "SocketIO.h"
#include "utils/Noncopyable.h"

class TcpConnection : public Noncopyable
{
public:
    TcpConnection(int _fd);
    ~TcpConnection();

    /**
     * @brief Receive a message from the connected socket.
     *
     * @return std::string The received message.
     */
    std::string recive();

    /**
     * @brief Send a message to the connected socket.
     *
     * @param _msg Message to send.
     */
    void send(std::string const& _msg);

private:
    SocketIO m_sockio;
};