#include "CCompletionPort.h"

bool CCompletionPort::InitServer()
{
    WSADATA     wsaDATA;


    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaDATA);

    if (0 != iResult)
    {
        cout << "[ Fail ] WSAStartUp() : " << WSAGetLastError() << "\n";
        return false;
    }

    //TCP Overlapped I/O家南 积己
    m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP or 0*/, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (m_ListenSocket == INVALID_SOCKET)
    {
        cout << "[ Fail ] WSASocket() : " << WSAGetLastError() << "\n";
        return false;
    }

    
    cout << "[ Success ] Socket Initialize" << "\n";
    return true;
}

bool CCompletionPort::Bind_Listen()
{

    SOCKADDR_IN stServAddr;
    stServAddr.sin_family = AF_INET;
    stServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stServAddr.sin_port = htons(PORT_NUM);


    int iResult= bind(m_ListenSocket, (SOCKADDR*)&stServAddr, sizeof(SOCKADDR_IN));
    if (0 != iResult)
    {
        cout << "[ Fail ] bind() : " << WSAGetLastError() << "\n";
        return false;
    }
    cout << "[ Success ] Socket Bind" << "\n";

    iResult = listen(m_ListenSocket, 5);
    if (0 != iResult)
    {
        cout << "[ Fail ] listen() : " << WSAGetLastError() << "\n";
        return false;
    }

    cout << "[ Success ] Server standby.." << "\n";
    return true;
}
