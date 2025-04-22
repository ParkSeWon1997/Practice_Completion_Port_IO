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

    //TCP Overlapped I/O���� ����
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
        _In_     DWORD     NumberOfConcurrentThreads        �ý��ۿ� �ִ� ���μ��� ����ŭ
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
                                                (PULONG_PTR)&pstClientSock, //�� ����� �Ϸ��� ������ ����
                                                &lpOverlappedIOData, //overlapped��ü�� �ޱ� ���� ������
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

        WSASend(pstClientSock->m_hClientSock,                                           //Overlapped ���� �ڵ�                       //non-overlapped ������ ���� �Ǹ� send �Լ��� ������ ������� ������ ����
            &(pstClientSock->m_stSendOverlappedEx.wsaBuf)                               //WSABUF ����ü �迭�� ����Ű�� ������        //���⼭�� ������ �����͸� ���ϴ� ���� ����
            , 1                                                                         //lpBuffers�� ����Ű�� WSABUF ����ü �迭�� ũ��
            , NULL                                                                      //������ ���۵� ����Ʈ ��                    //�񵿱� �����̸� ���� �����ص� ��
            , 0                                                                         //������ ���۹�� �ɼ�                       ��)��� ������� �����͸� �����ϴ� ���
            , (LPOVERLAPPED) &(pstClientSock->m_stSendOverlappedEx.overlapped)          //WSAOVERLAPPED����ü ���� ������           //��ø ����� �ƴ� ��� ���ڴ� ����
            , NULL);                                                                    //Completion Routine�� ������               //��ø ����� �ƴ� ��� ���ڴ� ����
        

    }

}

