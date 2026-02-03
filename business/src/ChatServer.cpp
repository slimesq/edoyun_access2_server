#include "ChatServer.h"
#include <cstdio>
#include <iostream>
#include <memory>
#include "Task.h"

ChatServer::ChatServer(size_t _threadNum,
                       size_t _queSize,
                       std::string const& _ip,
                       unsigned short _port)
    : m_pool(_threadNum, _queSize), m_server(_ip, _port)
{
}

ChatServer::~ChatServer()
{
}

void ChatServer::start()
{
    this->m_pool.start();

    this->m_server.setAllCallbacks(
        std::bind(&ChatServer::onNewConnection, this, std::placeholders::_1),
        std::bind(&ChatServer::onMessage, this, std::placeholders::_1),
        std::bind(&ChatServer::onClose, this, std::placeholders::_1));
    this->m_server.start();
}

void ChatServer::stop()
{
    this->m_server.stop();
    this->m_pool.stop();
}

void ChatServer::onNewConnection(std::shared_ptr<TcpConnection> const& _conn)
{
    std::cout << _conn->toString() << " has connected!" << std::endl;
}

void ChatServer::onMessage(std::shared_ptr<TcpConnection> const& _conn)
{
    // recv
    std::string recvMsg = _conn->recive();

    std::shared_ptr<Task> task{this->m_taskFactory(_conn, recvMsg)};
    if (!task)
    {
        ::perror("task is nullptr");
        return;
    }

    this->m_pool.addTask(std::bind(&Task::process, task));
}

void ChatServer::onClose(std::shared_ptr<TcpConnection> const& _conn)
{
    std::cout << _conn->toString() << " has closed!" << std::endl;
}

void ChatServer::setTaskFactory(TaskFactory _factory)
{
    this->m_taskFactory = std::move(_factory);
}