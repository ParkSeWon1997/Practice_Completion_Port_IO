

#pragma once
#include"define.h"

struct tagClientInfo
{
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





private:
	HANDLE	m_hCompletionPort = INVALID_HANDLE_VALUE;
	SOCKET	m_ListenSocket = INVALID_SOCKET;
};

