#include "Task.h"

Task::Task(std::shared_ptr<TcpConnection> const& _conn)
    : m_conn(_conn)
{
}
Task::Task(Task const& _task)
{
    if (this != &_task)
    {
        this->m_conn = _task.m_conn;
    }
}

Task::~Task()
{
}