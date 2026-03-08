#pragma once
#include "Task.h"
#include <string>

/**
 * @brief Task for handling group chat messages.
 *
 * @details Wraps the received message in Train(GroupChat) format and
 *          broadcasts it to all other connected clients.
 */
class MyTask : public Task
{
public:
    /**
     * @brief Construct a MyTask.
     *
     * @param _conn The sender's connection.
     * @param _recvMsg The received message payload.
     */
    MyTask(std::shared_ptr<TcpConnection> const& _conn, const std::string& _recvMsg);
    ~MyTask() override;

    /**
     * @brief Broadcast the message to all other connected clients in Train format.
     */
    void process() override;

private:
    std::string m_recvMsg;
    std::string m_sendMsg;
};
