#include <unistd.h>
#include <iostream>
#include "TcpConnection.h"
#include "TcpServer.h"

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
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallbacks(std::move(onNewConnection), std::move(onMessage), std::move(onClose));
    server.start();

    return 0;
}