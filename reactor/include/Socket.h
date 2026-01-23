#pragma once

#include "utils/Noncopyable.h"

class Socket : public Noncopyable
{
public:
    Socket();
    explicit Socket(int _fd);
    ~Socket();

    /**
     * @brief Get the socket file descriptor.
     *
     * @return int Socket file descriptor.
     */
    int getFd() const noexcept;

private:
    /**
     * @brief socket file descriptor.
     *
     */
    int m_fd{};
};