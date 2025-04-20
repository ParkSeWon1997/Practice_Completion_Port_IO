
#include"CCompletionPort.h"

int main()
{
    CCompletionPort IOServer;
    IOServer.InitServer();
    IOServer.Bind_Listen();

    return 0;
}

