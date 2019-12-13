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

#define SQL_NAME "root"         //本地MY_SQL 用户名
#define SQL_PASS "112508"       //本地MY_SQL 密码
#define SQL_DATABASE "im"       //本地数据库 database


void Connect_SQL(MYSQL& M) //连接本地化数据库
{
	//初始化数据库
	if (mysql_library_init(0, NULL, NULL)!=0)
	{
		cout << "数据库初始化失败" << endl;
		system("pause");
		return;
	}

	//初始话数据结构
	if (mysql_init(&M) == NULL)
	{
		cout << "mysql_init() fail" << endl;
		system("pause");
		return;
	}

	//数据库设置，字符集为中文
	if (mysql_options(&M, MYSQL_SET_CHARSET_NAME, "gbk") != 0)
	{
		cout << "mysql_options fail" << endl;
		system("pause");
		return;
	}

	//连接数据库
	if ((mysql_real_connect(&M, "localhost", SQL_NAME, SQL_PASS, SQL_DATABASE, 3306, NULL, 0)) == NULL)
	{
		cout << "mysql_real_connect() failed" << endl;
		system("pause");
		return;
	}
}

/*数据库存储结构
  ID（INT）  NAME（TEXT）  PASSWD（TEXT）  SIG（TEXT）  FRI（TEXT）  REG_T（DATATIME）

  FRI(好友列表） '10001 10002 10003';  以空格分隔进行存储
  
*/

class IM_MYSQL
{
	friend void Connect_SQL(MYSQL&);

public:
	IM_MYSQL():resault(NULL) { Connect_SQL(MSQ); }
	~IM_MYSQL() = default;

	int user_reg(user_mes&);                                        //注册
	int record_send_mes(chat_mes cmes);                             //记录发送的消息

	void alter_data(user_mes u_mes);                                //修改个人资料
	void do_alter_data(string ID,string key_word, string value);    //实际修改的操作函数
	void add_friend(chat_mes m);                                    //实际添加好友的数据库操作

	// 缺乏：void 推送给所有在线好友，信息更改,在逻辑中实现；没必要在数据库中做

	vector<user_mes> find_friend_list(string user_name);            //根据关键字 解析成名字或者ID， 返回模糊结果集，用于好友查找

	user_mes find_by_id(string ID);                                 //通过ID查询，获得用户结构体信息
	vector<user_mes> find_user_friends(string ID);                  //通过ID查询，返回【所有好友】结构体

	void add_off_line_mes(chat_mes c);                              //添加离线消息
	vector<chat_mes> get_off_line_mes(string ID);                   //查找某个ID的离线消息集导出，并且从库中删除

	void get_mes_text(string ID);                                   //导出某个号码的聊天记录,并写入到本地的 txt
	
	//以下为DEBUG用
	void print_mes();                                               //打印反馈信息
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
		unsigned long* lengths; // 存着每个数据格子的宽度；
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
	//查询： select ID from user order by `ID` desc limit 1;
	//insert into user(ID,NAME,PASSWD,SIG,FRI,REG_T) VALUES (100000,'yanpeng','112508','','',now())
	
	string ID;
	mysql_query(&MSQ, "select ID from user order by `ID` desc limit 1");
	MYSQL_RES* resault = mysql_store_result(&MSQ);

	//生成ID，如果没有就100000；
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
	//初始化时间
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
	   ID 必然存在
	   如果某个值非空，那么说明需要修改;
	   无需校验ID，直接查询修改即可。
	*/

	string s; //命令
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
	//返回信息结构体
	//查询指令 select * from user where ID=100000；

	string s("select * from user where ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //获取查询结果
	if (resault == NULL || resault->row_count==0) return user_mes(); // 无结果则返回空结构体

	unsigned int num_fields = mysql_num_fields(resault);//表格一共有几列数据
	MYSQL_ROW row;  //获取一行结果集
	row = mysql_fetch_row(resault);

	//结果集顺序：ID（INT）  NAME（TEXT）  PASSWD（TEXT）  SIG（TEXT）  FRI（TEXT）  REG_T（DATATIME）
	user_mes temp;
	temp.ID = row[0];
	temp.NAME = row[1];
	temp.SIG = row[3];

	return temp;
}

vector<user_mes> IM_MYSQL::find_user_friends(string ID)  //返回所有 好友结构体 列表
{
	
	string s("select * from user where ID=");
	s += ID;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //获取查询结果
	if (resault == NULL || resault->row_count == 0) return vector<user_mes>(); // 无结果则返回空结构体

	unsigned int num_fields = mysql_num_fields(resault);//表格一共有几列数据
	MYSQL_ROW row;  //获取一行结果集
	row = mysql_fetch_row(resault);

	string friends_ID = row[4];     //好友字符串 具体 ：  “123312  12321312”
	vector<user_mes> resault;

	stringstream ss(friends_ID);

	string s_t;

	while (ss >> s_t)
	{
		user_mes temp;
		temp = find_by_id(s_t);      // 查找每个好友的结构体
		resault.push_back(temp);
	}

	return resault;  // 返回结果
}

vector<user_mes> IM_MYSQL::find_friend_list(string key_word)
{

	/*
	    模糊查询，添加好友；
		识别关键字，如果纯数字，那么就作为ID查询
		如果非纯数字，那么就正则表达式，名字查询

		结果为一个vector<user_mes>
	*/

	//纯数字 简单查询即可
	if (key_word.find_first_of("0123456789") != string::npos)
	{
		vector<user_mes> temp;
		temp.push_back(find_by_id(key_word));
		return temp;
	}

	// 模糊查询名字 ，使用正则表达式
	// 指令：  select * from user where ID REGEXP '1'
	vector<user_mes> v;

	string s("select * from user where NAME REGEXP '");
	s += key_word;
	s += "'";
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //获取结果
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
	//添加离线消息
	// 命令:INSERT INTO TO_BE_SENT (SENT_TIME,SENT_ID,REC_ID,MES) VALUES (NOW(),'100001','100002','像弟子了');

	stringstream ss;
	ss << "INSERT INTO TO_BE_SENT (SENT_TIME,SENT_ID,REC_ID,MES) VALUES (";
	ss << get_time() << ",'" << c.sent_id << "','" << c.rec_id << "','" << c.chat_content <<"')";
	string s = ss.str();

	mysql_query(&MSQ, s.c_str());
	if (mysql_errno(&MSQ) != 0)
	{
		//记录错误日治；
	}

	return;
}

vector<chat_mes> IM_MYSQL::get_off_line_mes(string ID)
{
	//根据ID查询离线消息包
	//查询命令 SELECT * FROM TO_BE_SENT WHERE REC_ID='';

	vector<chat_mes> v;
	string s("SELECT * FROM TO_BE_SENT WHERE REC_ID=");
	s += ID;

	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //获取结果
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

void IM_MYSQL::get_mes_text(string ID) //导出某个号码的聊天记录,并写入到本地的 txt
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

	resault = mysql_store_result(&MSQ);  //获取结果
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
	// c1为被添加者，c2 为添加发起者
	// 操作为： 给c2的好友列表中 加入c1
	string c1 = m.rec_id;
	string c2 = m.sent_id;

	// 添加发起者的好友列表中，加入被添加者的信息
	string s("select * from user where ID=");
	s += c2;
	mysql_query(&MSQ, s.c_str());

	resault = mysql_store_result(&MSQ);  //获取查询结果
	if (resault == NULL || resault->row_count == 0) return; // 无结果则返回空结构体

	unsigned int num_fields = mysql_num_fields(resault);//表格一共有几列数据
	MYSQL_ROW row;  //获取结果集
	row = mysql_fetch_row(resault);

	string c2_friends = row[4];
	c2_friends += " ";
	c2_friends += c1;

	do_alter_data(c2, "FRI", c2_friends);

	return;
}