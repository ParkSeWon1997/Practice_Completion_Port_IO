
#include"CCompletionPort.h"

int main()
{
    CCompletionPort IOServer; //= (CCompletionPort*)malloc(sizeof(CCompletionPort));
    //new CCompletionPort();
    IOServer.InitServer();
    IOServer.Bind_Listen();
    IOServer.startServer();


    //while (1)
    //{
    //    //Some Loop....
    //}

    IOServer.DestroyThread();
    return 0;
}

