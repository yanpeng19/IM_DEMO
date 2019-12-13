#pragma once

#include <string>
#include <vector>

using namespace std;

struct user_mes;
struct chat_mes;


//�����Ϣ�ṹ��
struct pack_mes
{
	pack_mes() = default;
	pack_mes(user_mes u);
	pack_mes(chat_mes c);

	string s1;
	string s2;
	string s3;
	string s4;
};

// �û���Ϣ
struct user_mes
{
	//����һ�µȺţ�������Ϣʹ��
	user_mes operator=(user_mes u)
	{
		if (u.NAME != "") NAME = u.NAME;
		if (u.SIG != "") SIG = u.SIG;
		return *this;
	}

	user_mes(string s1, string s2, string s3, string s4) :ID(s1), NAME(s2), SIG(s3), PASSWORD(s4) {};
	user_mes() = default;
	user_mes(pack_mes u) :ID(u.s1), NAME(u.s2), SIG(u.s3),PASSWORD(u.s4){};
	string ID;
	string NAME;
	string SIG;
	string PASSWORD;

	bool empty() { return ID.empty(); };
};


// ������Ϣ
struct chat_mes
{
	chat_mes() = default;
	chat_mes(pack_mes u) :sent_id(u.s1), rec_id(u.s2),chat_content(u.s3),time(u.s4) {};
	string sent_id;
	string rec_id;
	string chat_content;   //���size 3000��
	string time;

	bool empty() { return sent_id.empty(); };
};

// ��½�ɹ�֮��������Ϣ
struct Login_mes
{
	user_mes user;
	vector<user_mes> friends;
	vector<chat_mes> off_line_mes;

	bool empty() { return user.empty(); }
};

//
//: s1(c.sent_id), s2(c.rec_id), s3(c.chat_content), s4(c.time) 

struct file_mes
{
	string file_name;
	string file_size;   // 15235435,10    ��С+����
	string hash;
	string index;
};