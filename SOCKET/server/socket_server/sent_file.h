#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <winsock2.h>

using namespace std;

int sent_file(SOCKET target,string file_name)
{
	ifstream f(file_name);
	if (!f.is_open())
	{
		cout << "无此文件，请检查路径" << endl;
		return -1;
	}
	
	f.seekg(0, ios::end);
	int size = f.tellg();
	f.seekg(0,ios::beg);

	char buff[10000] = { 0 };

	while (f.read(buff, 10000))
	{
		send(target, buff, 10000, 0);

	}
	
	string temp;

}