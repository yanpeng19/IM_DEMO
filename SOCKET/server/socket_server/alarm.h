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
�������࣬ѭ�����������Ƿ�������У����û��������ôͨ��һ����ʽ���ʼ�/�ֻ�������֪ͨ
*/

class mail
{
public:
	//mail() {intilizal(); };

	bool intilizal();     // ��ʼ������
	int send_mes(const string & s);       // ������Ϣ
	string recv_mes();    // ������Ϣ

private:
	SOCKET mail_server;  // ���������SOCKET
	sockaddr_in addr;    // ��������ַ��Ϣ
};


class alarm
{
public:
	alarm() :i(30){};
	int sent_error_mail(const string& s); // ���;�����Ϣ��ָ������

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
	//ʹ��SMTP ���;����ʼ�
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	mail_server = socket(AF_INET, SOCK_STREAM, 0);     //��ʼ����TCP��ʽ

	addr.sin_family = AF_INET;                         // ��ʼ�� ����������Ķ˿ں͵�ַ
	addr.sin_port = htons(MAIL_PORT);
	addr.sin_addr.s_addr = inet_addr(MAIL_IP);

	int f = connect(mail_server, (struct sockaddr*) & addr, sizeof(struct sockaddr));  //����

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
		SMTP���ʼ����̣�#�ַ������� \r\n����

		C�� connect
		S:  220  ��Ӧ
		C:  HELO
		S�� 250  һЩ˵��
		C:  auth login;

		S:  334 Ҫ���û���
		C:  base64 user_name;
		S:  334 Ҫ������
		C�� base64 user_passwrd;
		S:  235 ��֤�ɹ�

		C�� mail from: <xxx@sina.com>
		C:  rcpt to: <xxx@xx.com>

		C:  DATA 
		S:  354 
		C:  ����
		C:  .
		S:  250
		C:  quit
		S:  221
	
	*/
	string recv ;

	m.intilizal();  //��ʼ������SMTP , �����������������
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


	m.send_mes("MAIL FROM:<503444122@qq.com>\r\n"); //��������
	cout << m.recv_mes() << endl;

	m.send_mes("rcpt to:<503444122@qq.com>\r\n");  //��������
	cout << m.recv_mes() << endl;


	m.send_mes("data\r\n");
	cout << m.recv_mes() << endl;

	m.send_mes("From:\"yanpeng\"<503444122@qq.com>\r\n"); //������

	m.send_mes("to:\"С��\"<503444122@qq.com>\r\n");//������



	m.send_mes("subject:��Ӧ���Ǹ�����\r\n\r\n"); //����

	m.send_mes(" zhebu fen shi yingwen ceshi neirong!\r\n"); //����

	m.send_mes("\r\n.\r\n");
	recv = m.recv_mes();
	cout << recv << endl;




	m.send_mes("quit\r\n");
	cout << m.recv_mes() << endl;

	return 0;

}