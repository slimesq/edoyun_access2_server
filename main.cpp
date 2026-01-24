#include <unistd.h>
#include <iostream>
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"

// connection established
void onNewConnection(std::shared_ptr<TcpConnection> const& _conn)
{
    _conn->send("Welcome to server!\n");
}

// connection readable
void onMessage(std::shared_ptr<TcpConnection> const& _conn)
{
    std::string msg = _conn->recive();
    _conn->send(msg);  // echo back
}

// connection closed
void onClose(std::shared_ptr<TcpConnection> const& _conn)
{
    std::cout << "onClose: " << _conn->toString() << " has closed." << std::endl;
}

int main(int argc, char* argv[])
{
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();

    EventLoop eventLoop(acceptor);
    eventLoop.setOnConnectionCallback(onNewConnection);
    eventLoop.setOnMessageCallback(onMessage);
    eventLoop.setOnCloseCallback(onClose);

    eventLoop.loop();

    return 0;
}