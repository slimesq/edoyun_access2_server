#include "ChatServer.h"
#include <csignal>

int main(int argc, char* argv[])
{
    ::signal(SIGPIPE, SIG_IGN);

    ChatServer server(4, 10, "192.168.32.129", 8888);
    server.start();
    return 0;
}
