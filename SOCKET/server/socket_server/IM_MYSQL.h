#pragma once

#include <winsock.h>
#include <iostream>
#include <string>
#include <mysql.h>
#include <sstream>
#include "Im_message.h"
#include <sstream>
#include <time.h>
#include <fstream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")

#define SQL_NAME "root"         //����MY_SQL �û���
#define SQL_PASS "112508"       //����MY_SQL ����
#define SQL_DATABASE "im"       //�������ݿ� database


void Connect_SQL(MYSQL& M) //���ӱ��ػ����ݿ�
{
	//��ʼ�����ݿ�
	if (mysql_library_init(0, NULL, NULL)!=0)
	{
		cout << "���ݿ��ʼ��ʧ��" << endl;
		system("pause");
		return;
	}

	//��ʼ�����ݽṹ
	if (mysql_init(&M) == NULL)
	{
		cout << "mysql_init() fail" << endl;
		system("pause");
		return;
	}

	//���ݿ����ã��ַ���Ϊ����
	if (mysql_options(&M, MYSQL_SET_CHARSET_NAME, "gbk") != 0)
	{
		cout << "mysql_options fail" << endl;
		system("pause");
		return;
	}

	//�������ݿ�
	if ((mysql_real_connect(&M, "localhost", SQL_NAME, SQL_PASS, SQL_DATABASE, 3306, NULL, 0)) == NULL)
	{
		cout << "mysql_real_connect() failed" << endl;
		system("pause");
		return;
	}
}

/*���ݿ�洢�ṹ
  ID��INT��  NAME��TEXT��  PASSWD��TEXT��  SIG��TEXT��  FRI��TEXT��  REG_T��DATATIME��

  FRI(�����б� '10001 10002 10003';  �Կո�ָ����д洢
  
*/

class IM_MYSQL
{
	friend void Connect_SQL(MYSQL&);

public:
	IM_MYSQL():resault(NULL) { Connect_SQL(MSQ); }
	~IM_MYSQL() = default;

	int user_reg(user_mes&);                                        //ע��
	int record_send_mes(chat_mes cmes);                             //��¼���͵���Ϣ

	void alter_data(user_mes u_mes);                                //�޸ĸ�������
	void do_alter_data(string ID,string key_word, string value);    //ʵ���޸ĵĲ�������
	void add_friend(chat_mes m);                                    //ʵ����Ӻ��ѵ����ݿ����

	// ȱ����void ���͸��������ߺ��ѣ���Ϣ����,���߼���ʵ�֣�û��Ҫ�����ݿ�����

	vector<user_mes> find_friend_list(string user_name);            //���ݹؼ��� ���������ֻ���ID�� ����ģ������������ں��Ѳ���

	user_mes find_by_id(string ID);                                 //ͨ��ID��ѯ������û��ṹ����Ϣ
	vector<user_mes> find_user_friends(string ID);                  //ͨ��ID��ѯ�����ء����к��ѡ��ṹ��

	void add_off_line_mes(chat_mes c);                              //���������Ϣ
	vector<chat_mes> get_off_line_mes(string ID);                   //����ĳ��ID��������Ϣ�����������Ҵӿ���ɾ��

	void get_mes_text(string ID);                                   //����ĳ������������¼,��д�뵽���ص� txt
	
	//����ΪDEBUG��
	void print_mes();                                               //��ӡ������Ϣ
	void print_error() { cout << mysql_errno(&MSQ); }

private:
	MYSQL MSQ;
	MYSQL_RES* resault;
};

string get_time()
{
	time_t t = time(0);
	auto a = localtime(&t);
	stringstream ss;

	ss << "'";

	ss << (a->tm_year + 1900) << "-";

	if (a->tm_mon > 9) ss << a->tm_mon + 1 << "-";
	else ss << 0 << a->tm_mon + 1 << "-";

	if (a->tm_mday > 9) ss << a->tm_mday << " ";
	else ss << 0 << a->tm_mday << " ";

	if (a->tm_hour > 9) ss << a->tm_hour << ':';
	else ss << 0 << a->tm_hour << ':';

	if (a->tm_min > 9) ss << a->tm_min << ':';
	else ss << 0 << a->tm_min << ':';

	if (a->tm_sec > 9) ss << a->tm_sec;
	else ss << 0 << a->tm_sec;



	ss << "'";

	return ss.str();
}

void IM_MYSQL::print_mes()
{
	MYSQL_RES* resault = mysql_store_result(&MSQ);
	if (resault == NULL)
	{
		cout << "no resault" << endl;
		return;
	}
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i;

	num_fields = mysql_num_fields(resault);

	while ((row = mysql_fetch_row(resault)))
	{
		unsigned long* lengths; // ����ÿ�����ݸ��ӵĿ�ȣ�
		lengths = mysql_fetch_lengths(resault); 
		for (i = 0; i < num_fields; i++)
		{
			printf(" %.*s ", (int)lengths[i], row[i] ? row[i] : "NULL"); 
		}
		printf("\n");
	}

	mysql_free_result(resault);
}

int IM_MYSQL::user_reg(user_mes& mes)
{
	//��ѯ�� select ID from user order by `ID` desc limit 1;
	//insert into user(ID,NAME,PASSWD,SIG,FRI,REG_T) VALUES (100000,'yanpeng','112508','','',now())
	
	string ID;
	mysql_query(&MSQ, "select ID from user order by `ID` desc limit 1");
	MYSQL_RES* resault = mysql_store_result(&MSQ);

	//����ID�����û�о�100000��
	if (resault == NULL) ID = "100000";
	else
	{
		MYSQL_ROW row = mysql_fetch_row(resault);
		stringstream ss;
		ss << stoi(row[0]) + 1;
		ID = ss.str();
	}

	string s("insert into user(ID,NAME,PASSWD,SIG,FRI,REG_T) VALUES (");

	s += ID + "," + "'" + mes.NAME + "','" + mes.PASSWORD + "','','',now())";

	mysql_query(&MSQ, s.c_str());

	mysql_query(&MSQ, "select ID from user order by `ID` desc limit 1");
	resault = mysql_store_result(&MSQ);
	MYSQL_ROW row = mysql_fetch_row(resault);
	
	stringstream ss;
	ss << row[0];

	if (ss.str()!=ID)
	{
		cout << ss.str() << endl;
		cout << "error" << endl;
		system("pause");
		mysql_free_result(resault);
		return -1;
	}

	mysql_free_result(resault);
	return 0;
}

int IM_MYSQL::record_send_mes(chat_mes cmes)
{
	//��ʼ��ʱ��
	cmes.time = get_time();
	stringstream ss;
	ss << "insert into message(SENT_TIME,SENT_ID,REC_ID,MES) VALUES (";
	ss << cmes.time << "," << cmes.sent_id << "," << cmes.rec_id << "," << '\'' << cmes.chat_content << "')";
	
	string s;
	s == ss.str();
	mysql_query(&MSQ, s.c_str());

	return 0;
}

void IM_MYSQL::alter_data(user_mes u_mes)
{
	/* 
	   ID ��Ȼ����
	   ���ĳ��ֵ�ǿգ���ô˵����Ҫ�޸�;
	   ����У��ID��ֱ�Ӳ�ѯ�޸ļ��ɡ�
	*/

	string s; //����
	string ID = u_mes.ID;

	//UPDATE user SET NAME = 'Yanpeng' where ID = 100000;

	if (u_mes.NAME != "") do_alter_data(ID,"NAME", u_mes.NAME);
	if (u_mes.PASSWORD != "") do_alter_data(ID,"PASSWD", u_mes.PASSWORD);
	if (u_mes.SIG != "") do_alter_data(ID,"SIG", u_mes.SIG);

}

void IM_MYSQL::do_alter_data(string ID,string key_word, string value)
{
	//UPDATE user SET NAME = 'Yanpeng' where ID = 100000;
	stringstream ss;
	ss << "UPDATE user SET ";
	ss << key_word;
	ss << " ='";
	ss << value << "' where ID=" << ID;

	string s = ss.str();

	mysql_query(&MSQ, s.c_str());
	return;
}

user_mes IM_MYSQL::find_by_id(string ID)
{
	//������Ϣ�ṹ��
	//��ѯָ�� select * from user where ID=100000��

	string s("select * from user where ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ��ѯ���
	if (resault == NULL || resault->row_count==0) return user_mes(); // �޽���򷵻ؿսṹ��

	unsigned int num_fields = mysql_num_fields(resault);//���һ���м�������
	MYSQL_ROW row;  //��ȡһ�н����
	row = mysql_fetch_row(resault);

	//�����˳��ID��INT��  NAME��TEXT��  PASSWD��TEXT��  SIG��TEXT��  FRI��TEXT��  REG_T��DATATIME��
	user_mes temp;
	temp.ID = row[0];
	temp.NAME = row[1];
	temp.SIG = row[3];

	return temp;
}

vector<user_mes> IM_MYSQL::find_user_friends(string ID)  //�������� ���ѽṹ�� �б�
{
	
	string s("select * from user where ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ��ѯ���
	if (resault == NULL || resault->row_count == 0) return vector<user_mes>(); // �޽���򷵻ؿսṹ��

	unsigned int num_fields = mysql_num_fields(resault);//���һ���м�������
	MYSQL_ROW row;  //��ȡһ�н����
	row = mysql_fetch_row(resault);

	string friends_ID = row[4];     //�����ַ��� ���� ��  ��123312  12321312��
	vector<user_mes> resault;

	stringstream ss(friends_ID);

	string s_t;

	while (ss >> s_t)
	{
		user_mes temp;
		temp = find_by_id(s_t);      // ����ÿ�����ѵĽṹ��
		resault.push_back(temp);
	}

	return resault;  // ���ؽ��
}

vector<user_mes> IM_MYSQL::find_friend_list(string key_word)
{

	/*
	    ģ����ѯ����Ӻ��ѣ�
		ʶ��ؼ��֣���������֣���ô����ΪID��ѯ
		����Ǵ����֣���ô��������ʽ�����ֲ�ѯ

		���Ϊһ��vector<user_mes>
	*/

	//������ �򵥲�ѯ����
	if (key_word.find_first_of("0123456789") != string::npos)
	{
		vector<user_mes> temp;
		temp.push_back(find_by_id(key_word));
		return temp;
	}

	// ģ����ѯ���� ��ʹ��������ʽ
	// ָ�  select * from user where ID REGEXP '1'
	vector<user_mes> v;

	string s("select * from user where NAME REGEXP '");
	s += key_word;
	s += "'";
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ���
	if (resault == NULL || resault->row_count == 0) return vector<user_mes>();

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(resault)))
	{
		user_mes temp;
		temp.ID = row[0];
		temp.NAME = row[1];
		temp.SIG = row[3];
		v.push_back(temp);
	}

	return v;
}

void IM_MYSQL::add_off_line_mes(chat_mes c)
{
	//���������Ϣ
	// ����:INSERT INTO TO_BE_SENT (SENT_TIME,SENT_ID,REC_ID,MES) VALUES (NOW(),'100001','100002','�������');

	stringstream ss;
	ss << "INSERT INTO TO_BE_SENT (SENT_TIME,SENT_ID,REC_ID,MES) VALUES (";
	ss << get_time() << ",'" << c.sent_id << "','" << c.rec_id << "','" << c.chat_content <<"')";
	string s = ss.str();

	mysql_query(&MSQ, s.c_str());
	if (mysql_errno(&MSQ) != 0)
	{
		//��¼�������Σ�
	}

	return;
}

vector<chat_mes> IM_MYSQL::get_off_line_mes(string ID)
{
	//����ID��ѯ������Ϣ��
	//��ѯ���� SELECT * FROM TO_BE_SENT WHERE REC_ID='';

	vector<chat_mes> v;
	string s("SELECT * FROM TO_BE_SENT WHERE REC_ID=");
	s += ID;

	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ���
	if (resault == NULL || resault->row_count == 0) return vector<chat_mes>();

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(resault)))
	{
		chat_mes temp;
		temp.time = row[1];
		temp.sent_id = row[2];
		temp.rec_id = row[3];
		temp.chat_content = row[4];
		v.push_back(temp);
	}
	return v;
}

void IM_MYSQL::get_mes_text(string ID) //����ĳ������������¼,��д�뵽���ص� txt
{
	ofstream f;
	string path("mes_");
	path += ID;
	path += ".txt";
	f.open(path);
	if (f.is_open())
	{
		cout << "chenggong";
	}

	string s("SELECT * FROM TO_BE_SENT WHERE SENT_ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ���
	if (resault == NULL || resault->row_count == 0);
	else
	{
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(resault)))
		{
			f << "[" << row[1] << "]" << " [" << row[2] << "] [" << row[3] << "] [con:" << row[4] << "]" << endl;
		}
	}

	s = ("SELECT * FROM TO_BE_SENT WHERE REC_ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());
	if (resault == NULL || resault->row_count == 0);
	else
	{
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(resault)))
		{
			f << "[" << row[1] << "]" << " [" << row[2] << "] [" << row[3] << "] [con:" << row[4] << "]" << endl;
		}
	}

	f.close();

	return;
}

void IM_MYSQL::add_friend(chat_mes m)
{
	// c1Ϊ������ߣ�c2 Ϊ��ӷ�����
	// ����Ϊ�� ��c2�ĺ����б��� ����c1
	string c1 = m.rec_id;
	string c2 = m.sent_id;

	// ��ӷ����ߵĺ����б��У����뱻����ߵ���Ϣ
	string s("select * from user where ID=");
	s += c2;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //��ȡ��ѯ���
	if (resault == NULL || resault->row_count == 0) return; // �޽���򷵻ؿսṹ��

	unsigned int num_fields = mysql_num_fields(resault);//���һ���м�������
	MYSQL_ROW row;  //��ȡ�����
	row = mysql_fetch_row(resault);

	string c2_friends = row[4];
	c2_friends += " ";
	c2_friends += c1;

	do_alter_data(c2, "FRI", c2_friends);

	return;
}