#include "MyTask.h"
#include "TcpConnection.h"
#include "utils/TrainHelper.h"
#include <iostream>
#include <string>

MyTask::MyTask(std::shared_ptr<TcpConnection> const& _conn, std::string const& _msg)
    : Task(_conn), m_recvMsg(_msg)
{
}

MyTask::~MyTask()
{
}

void MyTask::process()
{
    // Wrap the message in Train(GroupChat) format for broadcasting
    this->m_sendMsg = TrainHelper::buildTrainMsg(MsgType::GroupChat, this->m_recvMsg);

    const std::map<int, std::shared_ptr<TcpConnection>> conns{this->m_conn->getTcpConnections()};
    for (auto [fd, connection] : conns)
    {
        if (fd != this->m_conn->getFd())
        {
            connection->sendInLoop(this->m_sendMsg);
        }
    }
}
