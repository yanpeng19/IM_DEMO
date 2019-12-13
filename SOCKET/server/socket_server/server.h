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


	void intialize_server();                                     // ��ʼ��������socket,��ַ,����bind �� Listen

	void run();                                                  // �����̣�����3���ӽ��̣� ���������Σ����

	void accpet_client();                                        // ѭ��accpet �ͻ��ˣ������̸߳��ͻ���SOCKET �������н�����Ϣ
	void listen_client(SOCKET client,bool &flag);                // �����ͻ��˺��������Ҹ���ͷ����Ϣ���д���


	// ����������ⲿ��
	void heart_judge();                                          // �������ʵ�ʲ��裺 ÿ300������ͻ��ˣ��ͷ�δ�����źſͻ���
	void heart_act(const string& ID)                             // ����ĳ���ͻ��˵�����������ʾ��Ȼ����״̬
	{
		heart_clients[ID] = TRUE;
	}

	// ����ҵ�񲿷�
	int respond_login(SOCKET client,char* buff);                  // ��Ӧ��½���󣬷��ͳɹ�����ʧ����Ϣ�ṹ��
	Login_mes get_login_mes(user_mes user);                       // ����ID���������½�Ľṹ�壻

	int transmit(chat_mes mse);                                   // ����ͻ��˷���������Ϣ�����Ҹ��ݽ������Ƿ����߽��д���

	int modify_data(user_mes);                                    // �޸�ĳ���û��ĸ�����Ϣ,��Ҫ�޸ĵ�����Ϊ�ǿգ�
	                                                              // �޸ĺ󣬲����������ߺ��ѷ��͸�����Ϣ,ͷ����ʾΪMODIFY_MES

	int sent_fri_cheat(SOCKET c,const string& key);               // �����û����ң�����ĳ���û������û�����Ϣ���ͻ���,��ռ�

	int add_friend_req(chat_mes chat);                            // ���ͺ������󵽿ͻ��ˣ� a->���͸�B 
	int dis_friend_req(chat_mes chat);                            // ���ݷ��ؽ��������������󣻣�ͨ��/��ͨ��������֪�����������
	                                                              // ���string �ǿգ���ô��ʾ���ܣ����STRINGΪ�գ���ʾ������
	                                                              // ��ͨ���򽫽����֪ A��ͨ����������ݿ⣬�� A�ĺ����б������ B 
	 
	int del_friend_req(const string& s);                          // ����ɾ���������󣬲���������֤��˫���໥ɾ��
	

private:
	IM_MYSQL SQL;                                                 // ���ݿ���
	SOCKET socket_server;                                         // ������socket
	struct sockaddr_in server_addr;                               // ��������ַ
	map<string, SOCKET> clients;                                  // SOCKET�ͻ���

	map<string, bool> heart_clients;                              // �������map�����Ϊ 0 ������ͻ���û�������ˣ��ͷ���Դ��
	                                                              // 300�����һ�Σ��� 0 �ĳ�Ա�ͷţ����ҽ����� ��Ա����Ϊ0
	                                                              // �ȴ��ͻ��˷�����������������Ϊ1

	hy



	string shell;                                                 // �������һЩ��������
	im_log system_log;                                            // ��������
	im_log error_log;                                             // ��������
	 
};

void server::intialize_server()
{
	WSAData wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup error" << endl;
		error_log.add_mes("WSA��ʼ������");
		exit;
	}

	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // socket ������

	if (sock_server == INVALID_SOCKET)
	{
		cout << "������ socket ��ʼ��ʧ��" << endl;
		error_log.add_mes("socket_server ��ʼ��ʧ��");
		exit;
	}

	struct sockaddr_in server_addr;  //��������ַ����Ϣ


	//��ʼ�������ַ�˿���Ϣ
	server_addr.sin_family = AF_INET;  //Э��
	server_addr.sin_port = htons(PORT); //�˿�
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);//ip

	//��
	if ((bind(sock_server, (struct sockaddr*) & server_addr, sizeof(struct sockaddr))) == SOCKET_ERROR)
	{
		cout << "�󶨴���" << endl;
		error_log.add_mes("server_socket bind error");
		exit;
	}

	//����
	if (listen(sock_server, 20) == SOCKET_ERROR)
	{
		cout << "��������" << endl;
		error_log.add_mes("server_socket listen error");
		exit;
	}
}

void server::run()
{
	// �����߳�
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

int server::respond_login(SOCKET client,char * buff)   // ��Ӧ��½���󣬳ɹ��򷢻ӵ�½�ṹ�壬ʧ�ܷ���ʧ�ܴ��룻
{
	unsigned index = 0;
	int t = 0;

	string friends;
	user_mes c = unpack_mes(buff, index);          //  ��ȡ��½��Ϣ
	user_mes res = SQL.find_by_id(c.ID);

	if (res.ID == "")                              //  ���˺���Ϣ
	{
		send(client, "8", 4, 0);
		return -1;
	}
	else if (res.PASSWORD != res.PASSWORD)         //  ���벻��ȷ
	{
		send(client, "8", 4, 0);
		return -1;
	}

	Login_mes client_log = get_login_mes(c);       //  ��ѯ���ݿ����ɽṹ��
	char buffer[MAX_STREAM_SIZE]{ 0 };

	pack_login_mes(client_log, buffer);
	int f = send(client, buffer, index, 0);

	return f;
}

Login_mes server::get_login_mes(user_mes u)  // ����ID���ɷ��ؽṹ��
{
	Login_mes mes;
	mes.user = u;
	mes.friends = SQL.find_user_friends(u.ID);
	mes.off_line_mes = SQL.get_off_line_mes(u.ID);

	return mes;
}

int server::del_friend_req(const string & s)      // ����ɾ���������󣬲������ݿ��У������ߴӺ����б���ɾ��������Է����ߣ������͸�����Ϣ
{
	return 0;
}

int server::transmit(chat_mes mes)
{
	auto p = clients.find(mes.rec_id);

	if (p == clients.end())                      //  �������������Ϣ��������Ϳ⣬������
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
	SQL.alter_data(user);    //  �޸��û����ݿ���Ϣ

	vector<user_mes> user_f = SQL.find_user_friends(user.ID);

	char buffer[MAX_STREAM_SIZE] = { 0 };

	unsigned size = 0;
	pack_user(user, buffer, size);
	memset(buffer, sizeof(int), MODIFY_MES); // �޸�����ʶͷ����ʶΪ�޸��û���Ϣ

	for (auto a : user_f)                    // ���������ߺ��ѷ��� �޸ĺ���Ϣ
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

	// ����Է����ߣ���ôʹ��100000ϵͳ�˺ŷ�����Ϣ�ṹ���ȥ
	// ��������ߣ������10000ϵͳ�˺Ŵ������б�

	auto p = clients.find(chat.rec_id);

	if (p == clients.end())
	{
		SQL.add_off_line_mes(chat);
		return 1;
	}
	
	unsigned size = 0;
	char buffer[MAX_STREAM_SIZE] = { 0 };
	pack_chat(chat, buffer, size);

	int t = ADD_FRI_REQUEST;  // ����ͷ����ʶ
	memcpy(buffer, &t, sizeof(t));
	size = send(p->second, buffer, size, 0);

	return size;
}

int server::dis_friend_req(chat_mes chat)
{
	//����string�����жϣ����string �ǿգ���ô��ʾ����,����Ӻ���
	if (!chat.chat_content.empty())
		SQL.add_friend(chat);

	// �������,�ͻ��˸���string�Ƿ���жϽ��
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
	memcpy(&t, buffer, sizeof(t));  // ����ͷ����ʶ
	
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

