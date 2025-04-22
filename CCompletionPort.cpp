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

    cout << "[ Success ] Socket Initialize\n";




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
    cout << "[ Success ] CompletionPort Object Create\n";

    GetSystemInfo(&SystemInfo);


    CreateThread();
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
    cout << "[ Success ] Socket Bind \n";

    iResult = listen(m_ListenSocket, 5);
    if (0 != iResult)
    {
        cout << "[ Fail ] listen() : " << WSAGetLastError() << "\n";
        return false;
    }

    cout << "[ Success ] Server standby..\n";
    return true;
}

bool CCompletionPort::startServer()
{
   

    return false;
}

void CCompletionPort::DestroyThread()
{
    m_IsWorkerRun = false;
    CloseHandle(m_hCompletionPort);


    for (auto& th : m_IOWorkerThreads)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
    closesocket(m_ListenSocket);
}

bool CCompletionPort::CreateThread()
{
    UINT    i = 0;
    for (i; i < SystemInfo.dwNumberOfProcessors  ; i++)
    {
        m_IOWorkerThreads.emplace_back([this]() { WorkerThread(); });
    }
    cout << "[ Start Overlapped IO Thread ]\n";

  
    return true;
}

void CCompletionPort::WorkerThread()
{
  
    tagClientInfo* pstClientSock = NULL;
    DWORD           dwByteIOSize = 0;
    LPOVERLAPPED    lpOverlappedIOData = NULL;

    BOOL        IsSuccess = TRUE;

    while (m_IsWorkerRun)
    {

        
        IsSuccess = GetQueuedCompletionStatus(m_hCompletionPort, 
                                                &dwByteIOSize, 
                                                (PULONG_PTR)&pstClientSock, //입 출력을 완료한 소켓의 정보
                                                &lpOverlappedIOData, //overlapped객체를 받기 위한 포인터
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

        tagOverlappedEx* pOverlappedEx = (tagOverlappedEx*)lpOverlappedIOData;

        memcpy(pstClientSock->m_stSendOverlappedEx.buffer, pOverlappedEx->buffer, dwByteIOSize);
        pstClientSock->m_stSendOverlappedEx.wsaBuf.len = dwByteIOSize;
        pstClientSock->m_stSendOverlappedEx.wsaBuf.buf = pstClientSock->m_stSendOverlappedEx.buffer;

        WSASend(pstClientSock->m_hClientSock,                                           //Overlapped 소켓 핸들                       //non-overlapped 소켓이 전달 되면 send 함수와 동일한 방식으로 데이터 전송
            &(pstClientSock->m_stSendOverlappedEx.wsaBuf)                               //WSABUF 구조체 배열을 가리키는 포인터        //여기서는 전송할 데이터를 지니는 버퍼 정보
            , 1                                                                         //lpBuffers가 가리키는 WSABUF 구조체 배열의 크기
            , NULL                                                                      //실제로 전송된 바이트 수                    //비동기 전송이면 값을 무시해도 됨
            , 0                                                                         //데이터 전송방식 옵션                       예)긴급 방식으로 데이터를 전송하는 경우
            , (LPOVERLAPPED) &(pstClientSock->m_stSendOverlappedEx.overlapped)          //WSAOVERLAPPED구조체 변수 포인터           //중첩 출력이 아닌 경우 인자는 무시
            , NULL);                                                                    //Completion Routine의 포인터               //중첩 출력이 아닌 경우 인자는 무시
        

    }

}

