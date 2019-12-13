// socket_client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include "Im_message.h"
#include "Im_user.h"
#include "MD5.h"
#include <fstream>
#include <regex>

#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

#define PORT 5000       //服务器socket 端口
//#define SERVER_IP "49.233.166.35"//服务器IP

#define SERVER_IP "127.0.0.1"

using namespace std;

int connect_server(SOCKET& server)
{
	WSAData wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup error" << endl;
		// 记录错误日治
		return -1;
	}

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // socket 描述符
	if (server == INVALID_SOCKET)
	{
		cout << "socket 初始化失败" << endl;
		// 记录错误日治
		return -1;
	}

	struct sockaddr_in server_addr;  //服务器地址和信息

	//初始化服务地址端口信息
	server_addr.sin_family = AF_INET;  //协议
	server_addr.sin_port = htons(PORT); //端口
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);//ip

	//连接服务器
	int f = connect(server, (struct sockaddr*) & server_addr, sizeof(struct sockaddr));

	if (f == -1)
	{
		cout << "连接失败" << endl;
		//记录错误
		return -1;
	}
	return 0;
}

Login_mes log_in(SOCKET& server, user_mes u)   //登陆函数，返回一个 登陆结构体
{
	char buffer[4000];

	memset(buffer, 4000, 0);
	// 初始化SOCKET
	
	if (connect_server(server) == -1)
	{
		cout << "SOCKET 链接失败" << endl;
		return Login_mes();
	}

	int len = recv(server, buffer, 4000, 0);
	if (len == 0)
	{
		std::cout << "连接已断开" << std::endl;
		return Login_mes();
	}
	else if (len == SOCKET_ERROR)
	{
		std::cout << "服务器未响应\n" << std::endl;
		return Login_mes();
	}

	int type_t;
	unsigned index = 0;

	Login_mes l_ms;
	int vec_u_size = 0; // vector<user_mes> 的数据个数
	int vec_c_size = 0; // vector<chat_mes> 的数据个数

	/*
	  获取登陆信息结构体 login_mes
	*/
	l_ms.user = unpack_mes(buffer, index);

	//获取后两个结构体的元素个数
	memcpy(&vec_u_size, buffer + index, sizeof(int));
	index += sizeof(int);
	memcpy(&vec_c_size, buffer + index, sizeof(int));
	index += sizeof(int);

	for (int i = 0; i < vec_u_size; i++)
	{
		//循环处理 获得 vector<user_mes>
		//里面除了头部的类型 type_t int 外； 后面连续4个int 分别是4个 string的拷贝size长度
		l_ms.friends.push_back(unpack_mes(buffer, index));
	}

	for (int i = 0; i < vec_c_size; i++)
	{
		//循环处理 获得 vector<user_mes>
		//里面除了头部的类型 type_t int 外； 后面连续4个int 分别是4个 string的拷贝size长度
		l_ms.off_line_mes.push_back(unpack_mes(buffer, index));
	}
}


void listen_mes(SOCKET server)
{
	//unsigned long ul = 0;
	//ioctlsocket(server, FIONBIO, &ul);
	char buf[201];

	while (1)
	{
		int len = recv(server, buf, 200, 0);
		if (len == 0)
		{
			std::cout << "连接已断开" << std::endl;
			continue;
		}
		else if (len == SOCKET_ERROR)
		{
			std::cout << "服务器未响应\n是否重试？ y/n" << std::endl;
			char a;
			cin >> a;
			if (a == 'y' || a == 'Y') continue;
			else if (a == 'n' || a == 'N') break;

			else cout << "输入错误，请重新输入" << endl;
		}
		buf[len] = 0;
		std::cout << "接受消息: " << buf << std::endl;
	}
}

int main()
{

	SOCKET server;
	SOCKET s2;

	//string s1, s2;
	//user_mes log;

	//Login_mes m = log_in(server, log);  // log_in 在 Im_user.h 中，获取从server获得登陆的信息
	//if (m.empty())
	//{
	//	cout << "初始化错误" << endl;
	//	exit;
	//}

	////初始化用户类
	//Im_user client(m,server);

	connect_server(server);
	connect_server(s2);


	char * buff = new char[19990641];
	int t = 0;
	ofstream f("modle.zip",ios::binary);


	int n = recv(server, buff, 19990641, 0);
	f.write(buff, 19990641);
	f.close();

	delete[] buff;



	//分子进程监听信息，循环读取消息（消息1.聊天消息->转发到QT/标准输出  消息2.好友更新消息->更新好友信息  消息3.查询结果列表->转发到QT/标准输出

	//另外一进程，循环读取输入 -> 之后替换成QT的某个槽函数  
	//如果


 	system("pause");  
	return 0;
}
