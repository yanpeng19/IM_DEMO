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

// �û���
class Im_user
{
public:
	Im_user() = default;
	Im_user(Login_mes m,SOCKET s) :user(m.user),server(s)
	{

		//��ʼ��ϵͳ֪ͨ����ID=100000
		friends.insert({ "100000",{"100000","100000","",""} });

		// ��ʼ�������б�
		for (auto temp_user : m.friends)
		{
			string ID = temp_user.ID;
			friends.insert({ ID, temp_user });
		}

		// ������ȡ������Ϣ
		for (auto temp : m.off_line_mes)
		{
			cout << get_mes(temp) << endl;
		}
	}
	 
	// ����Ϊҵ�񲿷�

	void listen_server();                      // ���������������ݲ�������Ӧ�Ĵ���

	string get_mes(chat_mes mes);              // ����һ�д������� ������Ϣ �� �Կո���м��
	void update_friend(user_mes mes);          // ����ĳ��������Ϣ
	int modify_data(user_mes mes);             // �޸ĸ�����Ϣ�����ҷ��͵�������,Ҫ���޸���Ϣ��   
	                                           // �޸���Ϣ�ķ�ʽ��������֤�Ϸ��Ժ󣬷���һ�� user_mes ��Ϣ��������

	int sent_mes(chat_mes mes);                // ����������Ϣ����ĳ��ID
	void find_friend(string ID);               // ����һ��string��ʹ��ID/�ṹ���ѯ���ѣ�����Ӽ�����������
	void print_friend_list(vector<pack_mes>);  // ת�������б�QT��ĳ����ʾ�� -> ��ʱ����
	int add_friend(string ID);                 // ������Ӻ�������
	int delete_friend(string ID);              // ɾ������������֪ͨ�Է�


	// �����������

	void heart_moitoring();                    // �ͻ���ÿ���Ӹ�����˷���һ������������ʾ��Ȼ���ߣ�����ͬʱ���SOCKET
									

	// �����ļ��������

	int sent_file();                           // �����ļ�
	string hash_file(ifstream f);              // ���ļ�����hash�������õ� hashֵ��Ȼ����HASHֵ�����ϴ�
	void get_file();                           // �����ļ������Ҵ��ڱ���·����

private:
	user_mes user;
	map<string,user_mes> friends;
	SOCKET server;
};

// ѭ�����������Ұ���ͷ�� int t ���д���
void Im_user::listen_server()
{
	
	while (1)
	{
		char buf[4000] = { 0 };

		int len = recv(server, buf, 4000, 0);

		if (len == 0)
		{
			std::cout << "�����ѶϿ�" << std::endl;
			continue;
		}
		else if (len == SOCKET_ERROR)
		{
			std::cout << "������δ��Ӧ\n�Ƿ����ԣ� y/n" << std::endl;
			char a;
			cin >> a;
			if (a == 'y' || a == 'Y') continue;
			else if (a == 'n' || a == 'N') break;
		}

		int t;
		memcpy(&t, buf, 4);

		switch (t)
		{
		case 1://case1 ���º�����Ϣ
		{
			//������Ҹ���
			unsigned index = 0;
			user_mes temp = unpack_mes(buf,index); 
			update_friend(temp); 
			break;
		}

		case 2://case 2 ���˸��㷢��Ϣ
		{
			unsigned index = 0;
			chat_mes temp = unpack_mes(buf, index);
			cout << get_mes(temp) << endl;
			break;
		}

		case 4: // ��ȡ��һ�������б�
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
		cout << "����ʧ��" << endl; //֮���滻��QT ĳ������
		return -1;
	}

	return i;
}

int Im_user::sent_mes(chat_mes mes)
{
	char buf[4000] = { 0 };
	chat_mes u = mes;
	pack_chat(mes, buf);

	int len = 20 + mes.sent_id.size() + mes.rec_id.size() + mes.chat_content.size() + mes.time.size(); //���ͳ��ȣ�Ϊ5��INT + ����

	int i = send(server, buf, len, 0);
	if (i == 0 || i == -1)
	{
		cout << "����ʧ��" << endl; //֮���滻��QT ĳ������
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
		//��¼����
		cout << "��ѯ�ַ�������" << endl;
		return;
	}
	memcpy(buf, KEY.c_str(), KEY.size());

	int len = 8 + KEY.size();//���ͳ���Ϊ 2��INT + ����
	int f = send(server, buf, 2000, 0);

	if (f == 0 || f == -1)
	{
		cout << "������Ϣʧ��" << endl;
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
		cout << "����ʧ��" << endl;
		// ���Ҽ�¼
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
		cout << "����ʧ��" << endl;
		// ���Ҽ�¼
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
		cout << "���ͼ��ʧ�ܣ����������Ƿ�����" << endl;
		//��QT��ִ�б�����ʾ��
	}
}