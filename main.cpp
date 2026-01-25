#include <unistd.h>
#include <iostream>
#include <memory>
#include "MyTask.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "ThreadPool.h"

// connection established
void onNewConnection(std::shared_ptr<TcpConnection> const& _conn)
{
    _conn->send("Welcome to server!\n");
}

// connection readable
void onMessage(std::shared_ptr<TcpConnection> const& _conn)
{
    // receive the data from the client.
    std::string msg = _conn->recive();

    // Process the business logic here.

    std::cout << "onMessage: received message from " << _conn->toString() << ": " << msg;

    // The business processing is completed. The result data will be sent to the client.
    _conn->send(msg);  // echo back
}

// connection closed
void onClose(std::shared_ptr<TcpConnection> const& _conn)
{
    std::cout << "onClose: " << _conn->toString() << " has closed." << std::endl;
}

int testReactor(int argc, char* argv[])
{
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallbacks(std::move(onNewConnection), std::move(onMessage), std::move(onClose));
    server.start();

    return 0;
}

int testThreadPool(int argc, char* argv[])
{
    ThreadPool threadPool(4, 10);
    // create and start the thread pool
    threadPool.start();

    // simulate adding tasks to the thread pool
    for (int i = 0; i < 20; ++i)
    {
        auto pTask{new MyTask()};
        threadPool.addTask(pTask);
    }

    // close the thread pool
    threadPool.stop();
    return 0;
}

int main(int argc, char* argv[])
{
    testThreadPool(argc, argv);
    return 0;
}