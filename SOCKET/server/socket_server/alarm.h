#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Base64.h"

#include "atlenc.h"
#include "winsock2.h"

#pragma comment(lib, "wsock32.lib")

#define MAIL_PORT 25
//#define MAIL_IP "123.126.45.161"
#define MAIL_IP "14.17.57.241"

// 7ff1c3b941b1a5dd

using namespace std;
/*
报警器类，循环检测服务器是否继续运行，如果没有运行那么通过一定方式（邮件/手机）进行通知
*/

class mail
{
public:
	//mail() {intilizal(); };

	bool intilizal();     // 初始化邮箱
	int send_mes(const string & s);       // 发送信息
	string recv_mes();    // 接受信息

private:
	SOCKET mail_server;  // 邮箱服务器SOCKET
	sockaddr_in addr;    // 服务器地址信息
};


class alarm
{
public:
	alarm() :i(30){};
	int sent_error_mail(const string& s); // 发送警告信息到指定邮箱

private:
	int i;
	mail m;
};

string mail::recv_mes()   
{
	char buff[1000] = { 0 };
	int ret = recv(mail_server, buff, 1000, 0);
	stringstream ss;

	ss << buff;
	return ss.str();
}

bool mail::intilizal()
{
	//使用SMTP 发送警告邮件
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	mail_server = socket(AF_INET, SOCK_STREAM, 0);     //初始化，TCP格式

	addr.sin_family = AF_INET;                         // 初始化 邮箱服务器的端口和地址
	addr.sin_port = htons(MAIL_PORT);
	addr.sin_addr.s_addr = inet_addr(MAIL_IP);

	int f = connect(mail_server, (struct sockaddr*) & addr, sizeof(struct sockaddr));  //连接

	if (f == SOCKET_ERROR) return 0;
	else return 1;
}

int mail::send_mes(const string & s)
{
	char buff[1000] = { 0 };
	memcpy(buff, s.c_str(), s.size());

	int i = send(mail_server, buff, s.size(), 0);
	return i;
}

int alarm::sent_error_mail(const string&s)
{
	/*
		SMTP发邮件过程：#字符串都以 \r\n结束

		C： connect
		S:  220  响应
		C:  HELO
		S： 250  一些说明
		C:  auth login;

		S:  334 要求用户名
		C:  base64 user_name;
		S:  334 要求密码
		C： base64 user_passwrd;
		S:  235 认证成功

		C： mail from: <xxx@sina.com>
		C:  rcpt to: <xxx@xx.com>

		C:  DATA 
		S:  354 
		C:  内容
		C:  .
		S:  250
		C:  quit
		S:  221
	
	*/
	string recv ;

	m.intilizal();  //初始化邮箱SMTP , 并且连接邮箱服务器
	cout << m.recv_mes() << endl;

	m.send_mes("HELO smtp.sina.com\r\n");
	cout << m.recv_mes() << endl;

	m.send_mes("auth login\r\n");
	cout << m.recv_mes() << endl;
	
	int n = recv.find_first_of(' ');
	cout << string(recv, 0, n + 1);
	cout << unpack_base64(string(recv, n + 1, recv.size() - n - 1)) << endl;;

	m.send_mes(pack_base64("503444122@qq.com") + "\r\n");

	recv = m.recv_mes();
	n = recv.find_first_of(' ');
	cout << string(recv, 0, n + 1);
	cout << unpack_base64(string(recv, n + 1, recv.size() - n - 1)) << endl;;

	//7ff1c3b941b1a5dd
	//qeheclylsczcbjfd
	m.send_mes(pack_base64("qeheclylsczcbjfd") + "\r\n");

	recv = m.recv_mes();
	cout << recv << endl;


	m.send_mes("MAIL FROM:<503444122@qq.com>\r\n"); //发送邮箱
	cout << m.recv_mes() << endl;

	m.send_mes("rcpt to:<503444122@qq.com>\r\n");  //接受邮箱
	cout << m.recv_mes() << endl;


	m.send_mes("data\r\n");
	cout << m.recv_mes() << endl;

	m.send_mes("From:\"yanpeng\"<503444122@qq.com>\r\n"); //发送人

	m.send_mes("to:\"小迪\"<503444122@qq.com>\r\n");//接收人



	m.send_mes("subject:这应该是个标题\r\n\r\n"); //标题

	m.send_mes(" zhebu fen shi yingwen ceshi neirong!\r\n"); //内容

	m.send_mes("\r\n.\r\n");
	recv = m.recv_mes();
	cout << recv << endl;




	m.send_mes("quit\r\n");
	cout << m.recv_mes() << endl;

	return 0;

}