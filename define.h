#pragma once
#pragma comment(lib,"ws2_32")
#include<winsock2.h>
#include<process.h>
#include <iostream>
#include <thread>
#include<vector>
using namespace std;


#define PORT_NUM 8080	//0 ~ 65535 :  0 ~ 1023 Well-Known Port 
//const u_short PORT_NUM = 11021;//0 ~ 65535 :  0 ~ 1023 Well-Known Port  

#define MAX_CLIENT_NUM 200