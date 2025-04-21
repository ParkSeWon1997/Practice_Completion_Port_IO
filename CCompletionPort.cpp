#include "CCompletionPort.h"

SYSTEM_INFO SystemInfo;



bool CCompletionPort::InitServer()
{
    WSADATA     wsaDATA;


    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaDATA);

    if (0 != iResult)
    {
        cout << "[ Fail ] WSAStartUp() : " << WSAGetLastError() << "\n";
        return false;
    }

    //TCP Overlapped I/O소켓 생성
    m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP or 0*/, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (m_ListenSocket == INVALID_SOCKET)
    {
        cout << "[ Fail ] WSASocket() : " << WSAGetLastError() << "\n";
        return false;
    }

    cout << "[ Success ] Socket Initialize" << "\n";




    /*
    HANDLE WINAPI CreateIoCompletionPort(
        _In_     HANDLE    FileHandle,
        _In_opt_ HANDLE    ExistingCompletionPort,
        _In_     ULONG_PTR CompletionKey,
        _In_     DWORD     NumberOfConcurrentThreads        시스템에 있는 프로세서 수만큼
        );
        https://learn.microsoft.com/ko-kr/windows/win32/fileio/createiocompletionport
    */
    m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL,0 , 0);

    if (NULL == m_hCompletionPort)
    {
        cout << "[ Fail ] CompletionPort Object No Create: " << GetLastError() << "\n";
        return false;
    }

    GetSystemInfo(&SystemInfo);
    cout << "[ Success ] CompletionPort Object Create" << "\n";
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

bool CCompletionPort::startServer()
{
    CreateThread();

    return false;
}

bool CCompletionPort::CreateThread()
{
    UINT    i = 0;
    for (i; i < SystemInfo.dwNumberOfProcessors  ; i++)
    {
        m_IOWorkerThreads.emplace_back([this]() { WorkerThread(); });
    }


    cout << "[ Start Thread...] " << "\n";
    return true;
}

void CCompletionPort::WorkerThread()
{
    tagClientInfo* pstClientSock = nullptr;
    DWORD           dwByteIOSize = 0;
    LPOVERLAPPED    lpOverlappedIOData = NULL;

    BOOL        IsSuccess = TRUE;

    while (m_IsWorkerRun)
    {

        IsSuccess = GetQueuedCompletionStatus(m_hCompletionPort, 
                                                &dwByteIOSize, 
                                                (PULONG_PTR)&pstClientSock, 
                                                &lpOverlappedIOData, 
                                                INFINITE);


        if (TRUE == IsSuccess && 0 == dwByteIOSize && NULL == lpOverlappedIOData)
        {
            m_IsWorkerRun = false;
            continue;
        }
        
        if (NULL == lpOverlappedIOData)
        {
            continue;
        }
    }

}
