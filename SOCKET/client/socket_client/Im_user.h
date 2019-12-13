#pragma once
#include <string>
#include <vector>
#include "Im_message.h"
#include <iostream>
#include <sstream>
#include <map>
#include "pack.h"
#include <regex>

#include <winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

using namespace std;

// 用户类
class Im_user
{
public:
	Im_user() = default;
	Im_user(Login_mes m,SOCKET s) :user(m.user),server(s)
	{

		//初始化系统通知对象：ID=100000
		friends.insert({ "100000",{"100000","100000","",""} });

		// 初始化好友列表
		for (auto temp_user : m.friends)
		{
			string ID = temp_user.ID;
			friends.insert({ ID, temp_user });
		}

		// 批量读取离线消息
		for (auto temp : m.off_line_mes)
		{
			cout << get_mes(temp) << endl;
		}
	}
	 
	// 以下为业务部分

	void listen_server();                      // 监听服务发来的数据并且做相应的处理

	string get_mes(chat_mes mes);              // 返回一行处解包后的 聊天消息 ： 以空格进行间隔
	void update_friend(user_mes mes);          // 更新某个好友信息
	int modify_data(user_mes mes);             // 修改个人信息，并且发送到服务器,要求修改信息；   
	                                           // 修改信息的方式：本地验证合法性后，发送一个 user_mes 消息到服务器

	int sent_mes(chat_mes mes);                // 发送聊天消息，到某个ID
	void find_friend(string ID);               // 发送一个string，使用ID/结构体查询好友；结果从监听函数返回
	void print_friend_list(vector<pack_mes>);  // 转发好友列表到QT的某个提示框 -> 暂时不做
	int add_friend(string ID);                 // 发送添加好友请求
	int delete_friend(string ID);              // 删除好友且无需通知对方


	// 以下心跳检测

	void heart_moitoring();                    // 客户端每分钟给服务端发送一次心跳包，表示依然在线；并且同时检测SOCKET
									

	// 以下文件传输相关

	int sent_file();                           // 发送文件
	string hash_file(ifstream f);              // 对文件进行hash处理，并得到 hash值；然后以HASH值进行上传
	void get_file();                           // 接受文件，并且存在本地路径中

private:
	user_mes user;
	map<string,user_mes> friends;
	SOCKET server;
};

// 循环监听，并且按照头部 int t 进行处理
void Im_user::listen_server()
{
	
	while (1)
	{
		char buf[4000] = { 0 };

		int len = recv(server, buf, 4000, 0);

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
		}

		int t;
		memcpy(&t, buf, 4);

		switch (t)
		{
		case 1://case1 更新好友信息
		{
			//解包并且更新
			unsigned index = 0;
			user_mes temp = unpack_mes(buf,index); 
			update_friend(temp); 
			break;
		}

		case 2://case 2 有人给你发消息
		{
			unsigned index = 0;
			chat_mes temp = unpack_mes(buf, index);
			cout << get_mes(temp) << endl;
			break;
		}

		case 4: // 获取了一个好友列表
		{
			unsigned index = 0;
			index = 4;
			vector<pack_mes> v = unpack_vector(buf, index);
			//print_friend_list(v);
			break;
		}
		case 8:
		{
			  
		}

		}
	}
}

string Im_user::get_mes(chat_mes mes)
{
	stringstream ss;
	ss << mes.time << " " << mes.rec_id << " " << mes.sent_id << " " << mes.chat_content;
	return ss.str();
}

void Im_user::update_friend(user_mes mes)
{
	string ID = mes.ID;
	auto f = friends.find(ID);
	f->second = mes;
	return;
}

int Im_user::modify_data(user_mes mes)
{
	char buf[4000] = { 0 };

	pack_user(mes, buf);
	int i = send(server, buf, 4000, 0);
	if (i == 0 || i == -1)
	{
		cout << "发送失败" << endl; //之后替换成QT 某个内容
		return -1;
	}

	return i;
}

int Im_user::sent_mes(chat_mes mes)
{
	char buf[4000] = { 0 };
	chat_mes u = mes;
	pack_chat(mes, buf);

	int len = 20 + mes.sent_id.size() + mes.rec_id.size() + mes.chat_content.size() + mes.time.size(); //发送长度，为5个INT + 内容

	int i = send(server, buf, len, 0);
	if (i == 0 || i == -1)
	{
		cout << "发送失败" << endl; //之后替换成QT 某个内容
		return -1;
	}

	return i;
}

void Im_user::find_friend(string KEY)
{
	char buf[2000] = { 0 };
	int t = 5;

	memcpy(buf, &t, 4);
	if (KEY.size() > 1990)
	{
		//记录错误
		cout << "查询字符串过长" << endl;
		return;
	}
	memcpy(buf, KEY.c_str(), KEY.size());

	int len = 8 + KEY.size();//发送长度为 2个INT + 内容
	int f = send(server, buf, 2000, 0);

	if (f == 0 || f == -1)
	{
		cout << "发送信息失败" << endl;
	}
	return;
}

int Im_user::add_friend(string ID)
{
	char buf[40] = { 0 };
	int t = 6;
	string s = this->user.ID;
	s += " ";
	s += ID;

	pack_string(t, s, buf);

	int len = 8 + s.size();

	int f = send(server, buf, len, 0);

	if (f == 0 || f == -1)
	{
		cout << "发送失败" << endl;
		// 并且记录
		return -1;
	}
	return 0;
}

int Im_user::delete_friend(string ID)
{
	char buf[40] = { 0 };
	int t = 7;
	string s = this->user.ID;
	s += " ";
	s += ID;

	pack_string(t, s, buf);

	int len = 8 + s.size();

	int f = send(server, buf, len, 0);

	if (f == 0 || f == -1)
	{
		cout << "发送失败" << endl;
		// 并且记录
		return -1;
	}
	return 0;
}

void Im_user::heart_moitoring()
{
	int t = 99;
	char buf[100];

	memcpy(buf, &t, sizeof(int));
	int f = send(server, buf, sizeof(int), 0);

	if (f == -1)
	{
		cout << "发送检测失败，请检查网络是否连接" << endl;
		//在QT中执行报错提示框
	}
}