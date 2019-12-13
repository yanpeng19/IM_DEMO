// socket_server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "sent_file.h"
#include <time.h>
#include "alarm.h"
#include "im_message.h"
#include "IM_MYSQL.h"
#include "server.h"
#include <regex>

#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

using namespace std;

#define PORT 5000
#define SERVER_IP "127.0.0.1"

//监听转发函数，监听1，发送到2；
void send_mes(SOCKET client1, SOCKET client2)
{
	//unsigned long ul = 0;
	//ioctlsocket(client1, FIONBIO, &ul);
	char buf[201];

	while (1)
	{
		int f = recv(client1, buf, 200, 0);
		if (f != 0 && f != -1)
			send(client2, buf, f, 0);
		else cout << "接收消息失败" << endl;
	}
}


int main()
{
	WSAData wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup error" << endl;
		system("pause");
		return 0;
	}

	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // socket 描述符

	if (sock_server == INVALID_SOCKET)
	{
		cout << "服务器 socket 初始化失败" << endl;
		system("pause");
		return 0;
	}


	struct sockaddr_in server_addr;  //服务器地址和信息


	//初始化服务地址端口信息
	server_addr.sin_family = AF_INET;  //协议
	server_addr.sin_port = htons(PORT); //端口
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);//ip

	//绑定
	if ((bind(sock_server, (struct sockaddr*) & server_addr, sizeof(struct sockaddr))) == SOCKET_ERROR)
	{
		cout << "绑定错误" << endl;
		system("pause");
		return 0;
	}

	//监听
	if (listen(sock_server, 20) == SOCKET_ERROR)
	{
		cout << "监听错误" << endl;
		system("pause");
		return 0;
	}

	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);

	//连接
	cout << "接收客户端连接中" << endl;
	cout << "开放端口" << PORT << endl;
	cout << "绑定IP:" << SERVER_IP << endl;

	SOCKET client1 = accept(sock_server, &clntAddr, &nSize);
	//SOCKET client2 = accept(sock_server, &clntAddr, &nSize);

	//cout << client2 << endl;

	//sent_file(client1, "model.zip");

	ifstream f("k.zip", ios::binary);
	ofstream o("k_back.zip", ios::binary);

	if (!f.is_open())
	{
		cout << "无此文件，请检查路径" << endl;
		system("pause");
		return -1;
	}

	f.seekg(0, f.end);
	int length = f.tellg();
	f.seekg(0, f.beg);

	char * buff = new char[length];

	f.read(buff, length);
	o.write(buff, length);

	//int n = send(client1, buff, length,0);

	delete[] buff;


	f.close();
	o.close();
	
 	return 0;
}
