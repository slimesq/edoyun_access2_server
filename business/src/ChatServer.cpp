#include "ChatServer.h"
#include "MyTask.h"
#include "FileUploadTask.h"
#include "FileDownloadTask.h"
#include "utils/MsgType.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

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
    // Read Train header: length (size_t, 8 bytes) + msgType (int, 4 bytes)
    size_t length = 0;
    int msgTypeInt = 0;

    ssize_t ret = _conn->recvn(&length, sizeof(size_t));
    if (ret <= 0)
    {
        return;
    }

    ret = _conn->recvn(&msgTypeInt, sizeof(int));
    if (ret <= 0)
    {
        return;
    }

    // Read payload
    std::string payload;
    if (length > 0)
    {
        payload.resize(length);
        ret = _conn->recvn(payload.data(), length);
        if (ret <= 0)
        {
            return;
        }
    }

    MsgType msgType = static_cast<MsgType>(msgTypeInt);

    std::shared_ptr<Task> task;
    switch (msgType)
    {
    case MsgType::GroupChat:
        task = std::make_shared<MyTask>(_conn, payload);
        break;
    case MsgType::UploadBegin:
    case MsgType::UploadChunk:
        task = std::make_shared<FileUploadTask>(_conn, msgType, payload);
        break;
    case MsgType::DownloadBegin:
        task = std::make_shared<FileDownloadTask>(_conn, payload);
        break;
    default:
        std::cerr << "Unknown message type: " << msgTypeInt << std::endl;
        return;
    }

    this->m_pool.addTask(std::bind(&Task::process, task));
}

void ChatServer::onClose(std::shared_ptr<TcpConnection> const& _conn)
{
    std::cout << _conn->toString() << " has closed!" << std::endl;
}
