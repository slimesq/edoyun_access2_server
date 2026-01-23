#pragma once

#include "utils.h/Noncopyable.h"

class Socket : public Noncopyable
{
public:
    Socket();
    explicit Socket(int _fd);
    ~Socket();
    int getFd() const noexcept;

private:
    int m_fd{};
};