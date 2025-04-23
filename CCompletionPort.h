

#pragma once
#include"define.h"




class CCompletionPort
{
public:
	CCompletionPort() {};
	~CCompletionPort() { WSACleanup(); };



public:
	bool InitServer();
	bool Bind_Listen();
	bool startServer();
	void DestroyThread();


private:
	bool CreateThread();
	void WorkerThread();
	void AcceptClient();

	



private:
	HANDLE	m_hCompletionPort = INVALID_HANDLE_VALUE;
	SOCKET	m_ListenSocket = INVALID_SOCKET;



	bool		m_IsWorkerRun = true;
	vector<thread> m_IOWorkerThreads;
};

