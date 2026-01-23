#include <unistd.h>
#include <iostream>
#include "Acceptor.h"
#include "TcpConnection.h"

int main(int argc, char* argv[])
{
    Acceptor acceptor(argv[1], atoi(argv[2]));
    acceptor.ready();
    std::cout << "Server is running..." << std::endl;
    while (1)
    {
        int connfd = acceptor.accept();
        if (connfd < 0)
        {
            ::sleep(1);
            continue;
        }
        else
        {
            TcpConnection tcpConn(connfd);
            tcpConn.send("hello world\n");
        }
    }

    return 0;
}