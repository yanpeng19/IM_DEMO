#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <time.h> 

using namespace std;
/*
im_log 类，记录服务器操作和服务器错误
*/
class im_log
{
public:
	im_log() = delete;

	//使用文件地址和最大存储消息初始化
	im_log(const string& addr, const int& max_mes_num) : file_addr(addr), max_size(max_mes_num) {};
	int add_mes(const string&);
	void set_max(const int& i) { max_size = i; };

private:
	int max_size;
	string file_addr;
	ofstream fout;
	queue<string> mes;
};

int im_log::add_mes(const string& s)
{
	time_t t;
	t = time(0);
	cout << t << endl;
	tm* pt = localtime(&t);
	stringstream ss;
	ss << 1900 + pt->tm_year << "-" << 1 + pt->tm_mon << "-" << pt->tm_mday << " " << pt->tm_hour << ":" << pt->tm_min << ":" << pt->tm_sec << " " << s;
	mes.push(ss.str());

	// 流中存储数据超过一定数量就写入文件；
	if (mes.size() > max_size)
	{
		fout.open(file_addr,ios::app);
		while (!mes.empty())
		{
			fout << mes.front() << endl;
			mes.pop();
		}
		fout.close();
	}

	return 0;
}


