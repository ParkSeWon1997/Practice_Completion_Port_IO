#pragma once
#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<process.h>
#include <iostream>


struct tagClientInfo
{
	//SOCKET	
};



class CCompletionPort
{
public:
		CCompletionPort();
		~CCompletionPort();



public:
	bool InitServer();




};

