#pragma once

#include <memory>
#include "TcpConnection.h"

class Task
{
public:
    Task(std::shared_ptr<TcpConnection> const& _conn);
    Task(Task const& _task);
    virtual ~Task() = 0;

    /**
     * @brief Pure virtual functions, that is, the tasks to be executed.
     *
     */
    virtual void process() = 0;

protected:
    std::shared_ptr<TcpConnection> m_conn;
};