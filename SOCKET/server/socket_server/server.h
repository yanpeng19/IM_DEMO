#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <mysql.h>
#include "log.h"
#include "MD5.h"
#include "mes_stand.h"
#include "pack.h"
#include "IM_MYSQL.h"
#include <windows.h>

using namespace std;

class server
{
public:
	server(IM_MYSQL m) :SQL(m),system_log("/system_log.txt",5),
		error_log("/error_log.txt", 1) {
		intialize_server();
	};


	void intialize_server();                                     // 初始化服务器socket,地址,进行bind 和 Listen

	void run();                                                  // 主进程，进行3个子进程： 监听，日治，监控

	void accpet_client();                                        // 循环accpet 客户端，并分线程给客户端SOCKET ，并进行接受消息
	void listen_client(SOCKET client,bool &flag);                // 监听客户端函数，并且根据头部信息进行处理


	// 以下心跳检测部分
	void heart_judge();                                          // 心跳检测实际步骤： 每300秒遍历客户端，释放未发送信号客户端
	void heart_act(const string& ID)                             // 激活某个客户端的心跳包，表示依然连接状态
	{
		heart_clients[ID] = TRUE;
	}

	// 以下业务部分
	int respond_login(SOCKET client,char* buff);                  // 回应登陆请求，发送成功或者失败信息结构体
	Login_mes get_login_mes(user_mes user);                       // 根据ID，返回其登陆的结构体；

	int transmit(chat_mes mse);                                   // 处理客户端发送来的消息，并且根据接收者是否在线进行处理

	int modify_data(user_mes);                                    // 修改某个用户的个人信息,需要修改的内容为非空；
	                                                              // 修改后，并向所有在线好友发送更改信息,头部表示为MODIFY_MES

	int sent_fri_cheat(SOCKET c,const string& key);               // 反馈用户查找，发送某个用户或者用户集信息到客户端,或空集

	int add_friend_req(chat_mes chat);                            // 发送好友请求到客户端； a->发送给B 
	int dis_friend_req(chat_mes chat);                            // 根据返回结果，处理好友请求；（通过/不通过）并告知结果给申请人
	                                                              // 如果string 非空，那么表示接受；如果STRING为空，表示不接受
	                                                              // 不通过则将结果告知 A，通过则更改数据库，将 A的好友列表中添加 B 
	 
	int del_friend_req(const string& s);                          // 处理删除好友请求，并且无需验证，双方相互删除
	

private:
	IM_MYSQL SQL;                                                 // 数据库类
	SOCKET socket_server;                                         // 服务器socket
	struct sockaddr_in server_addr;                               // 服务器地址
	map<string, SOCKET> clients;                                  // SOCKET客户端

	map<string, bool> heart_clients;                              // 心跳检测map，如果为 0 则表明客户端没有连接了，释放资源；
	                                                              // 300秒遍历一次，将 0 的成员释放，并且将所有 成员设置为0
	                                                              // 等待客户端发来心跳包，再重置为1

	hy



	string shell;                                                 // 主程序的一些调试命令
	im_log system_log;                                            // 运行日治
	im_log error_log;                                             // 错误日治
	 
};

void server::intialize_server()
{
	WSAData wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup error" << endl;
		error_log.add_mes("WSA初始化错误");
		exit;
	}

	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // socket 描述符

	if (sock_server == INVALID_SOCKET)
	{
		cout << "服务器 socket 初始化失败" << endl;
		error_log.add_mes("socket_server 初始化失败");
		exit;
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
		error_log.add_mes("server_socket bind error");
		exit;
	}

	//监听
	if (listen(sock_server, 20) == SOCKET_ERROR)
	{
		cout << "监听错误" << endl;
		error_log.add_mes("server_socket listen error");
		exit;
	}
}

void server::run()
{
	// 心跳线程
	thread t1(this->heart_judge);
	t1.detach();

	thread t2(this->accpet_client);
	t2.detach();


}

void server::accpet_client()
{
	while (shell!="exit_server")
	{
		int n = sizeof(SOCKADDR);
		SOCKET temp = accept(socket_server, 0, &n);
		
	}
}

void server::listen_client(SOCKET socket,bool & flag)
{
	
}

int server::respond_login(SOCKET client,char * buff)   // 回应登陆请求，成功则发挥登陆结构体，失败发挥失败代码；
{
	unsigned index = 0;
	int t = 0;

	string friends;
	user_mes c = unpack_mes(buff, index);          //  获取登陆信息
	user_mes res = SQL.find_by_id(c.ID);

	if (res.ID == "")                              //  无账号信息
	{
		send(client, "8", 4, 0);
		return -1;
	}
	else if (res.PASSWORD != res.PASSWORD)         //  密码不正确
	{
		send(client, "8", 4, 0);
		return -1;
	}

	Login_mes client_log = get_login_mes(c);       //  查询数据库生成结构体
	char buffer[MAX_STREAM_SIZE]{ 0 };

	pack_login_mes(client_log, buffer);
	int f = send(client, buffer, index, 0);

	return f;
}

Login_mes server::get_login_mes(user_mes u)  // 根据ID生成返回结构体
{
	Login_mes mes;
	mes.user = u;
	mes.friends = SQL.find_user_friends(u.ID);
	mes.off_line_mes = SQL.get_off_line_mes(u.ID);

	return mes;
}

int server::del_friend_req(const string & s)      // 处理删除好友请求，并在数据库中，将两者从好友列表中删除，如果对方在线，则推送更改消息
{
	return 0;
}

int server::transmit(chat_mes mes)
{
	auto p = clients.find(mes.rec_id);

	if (p == clients.end())                      //  不在线情况，信息存入待发送库，并返回
	{
		SQL.add_off_line_mes(mes);
		return 0;
	}
	else
	{
		char buffer[MAX_STREAM_SIZE] = { 0 };
		unsigned size = 0;
		pack_chat(mes, buffer, size);
		if (size == 0) cout << " tranmit error" << endl;
		send(p->second, buffer, size,0);
		return size;
	}

}

int server::modify_data(user_mes user)
{
	SQL.alter_data(user);    //  修改用户数据库信息

	vector<user_mes> user_f = SQL.find_user_friends(user.ID);

	char buffer[MAX_STREAM_SIZE] = { 0 };

	unsigned size = 0;
	pack_user(user, buffer, size);
	memset(buffer, sizeof(int), MODIFY_MES); // 修改流标识头，标识为修改用户信息

	for (auto a : user_f)                    // 向所有在线好友发送 修改后信息
	{
		auto c = clients.find(a.ID);
		if (c != clients.end())      
		{
			send(c->second, buffer, size, 0);
		}
	}

	return size;
}

int server::sent_fri_cheat(SOCKET c,const string & s)
{
	vector<user_mes> users = SQL.find_friend_list(s);

	char buffer[MAX_STREAM_SIZE] = { 0 };
	unsigned size = 0;

	pack_vector(users, buffer, size);
	send(c, buffer, size, 0);

	return size;
}

int server::add_friend_req(chat_mes chat)
{

	chat.sent_id = SYSTEM_ADMIN;

	// 如果对方在线，那么使用100000系统账号发送信息结构体过去
	// 如果不在线，则加入10000系统账号待发送列表

	auto p = clients.find(chat.rec_id);

	if (p == clients.end())
	{
		SQL.add_off_line_mes(chat);
		return 1;
	}
	
	unsigned size = 0;
	char buffer[MAX_STREAM_SIZE] = { 0 };
	pack_chat(chat, buffer, size);

	int t = ADD_FRI_REQUEST;  // 更改头部标识
	memcpy(buffer, &t, sizeof(t));
	size = send(p->second, buffer, size, 0);

	return size;
}

int server::dis_friend_req(chat_mes chat)
{
	//根据string内容判断，如果string 非空，那么表示接受,并添加好友
	if (!chat.chat_content.empty())
		SQL.add_friend(chat);

	// 打包发回,客户端根据string是否空判断结果
	chat.rec_id = chat.sent_id;
	chat.sent_id = SYSTEM_ADMIN;
	auto p = clients.find(chat.rec_id);

	if (p == clients.end())
	{
		SQL.add_off_line_mes(chat);
		return 1;
	}

	unsigned size = 0;
	char buffer[MAX_STREAM_SIZE] = { 0 };
	pack_chat(chat, buffer, size);
	int t = ADD_FRI_RESAULT;
	memcpy(&t, buffer, sizeof(t));  // 更改头部标识
	
	size = send(p->second, buffer, size, 0);
	return size;
}

void server::heart_judge()
{
	while (1)
	{
		Sleep(300000);
		for(auto a :heart_clients)
			if (!a.second)
			{
				closesocket(clients[a.first]);
				clients.erase(a.first);
			}
			else a.second = FALSE;
	}
}

