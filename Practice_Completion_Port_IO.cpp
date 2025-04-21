
#include"CCompletionPort.h"

int main()
{
    CCompletionPort IOServer;
    IOServer.InitServer();
    IOServer.Bind_Listen();
    IOServer.startServer();

    return 0;
}

