#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include "MyTask.h"
#include <unistd.h>
#include "TcpConnection.h"

MyTask::MyTask(std::shared_ptr<TcpConnection> const& _conn, std::string const& _msg)
    : Task(_conn),m_recvMsg(_msg)
{
}

MyTask::~MyTask()
{
}

// Tasks need to be added based on the specific business logic.
void MyTask::process()
{
    // ::srand(::clock());              // seed the random number generator
    // int number{::rand() % 100 + 1};  // random sleep time between 1 and 5 seconds
    // std::cout << "MyTask number =  " << number << std::endl;
    // ::sleep(1);
    this->m_sendMsg = this->m_recvMsg;
    const std::map<int, std::shared_ptr<TcpConnection>> conns{this->m_conn->getTcpConnections()};
    for (auto [fd, connection] : conns)
    {
        if (fd != this->m_conn->getFd())
        {
            connection->sendInLoop(this->m_sendMsg);
        }
    }
}