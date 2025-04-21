

#pragma once
#include"define.h"

struct tagClientInfo
{
	SOCKET	m_hClientSock;



	tagClientInfo()
	{
		m_hClientSock = INVALID_SOCKET;
	}
	//SOCKET	
};



class CCompletionPort
{
public:
	CCompletionPort() {};
	~CCompletionPort() { WSACleanup(); };



public:
	bool InitServer();
	bool Bind_Listen();
	bool startServer();
	


private:
	bool CreateThread();
	void WorkerThread();
	


private:
	HANDLE	m_hCompletionPort = INVALID_HANDLE_VALUE;
	SOCKET	m_ListenSocket = INVALID_SOCKET;



	bool		m_IsWorkerRun = true;
	vector<thread> m_IOWorkerThreads;
};

