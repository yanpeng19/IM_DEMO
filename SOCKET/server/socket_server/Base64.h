#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>

using namespace std;

const unsigned char base64[64] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

/*
ASCII  "user"   [0111 0101] [0111 0011] [0110 0101]  [0111 0010]              //  3转4 + 1转4  共8*8 64字节
                     u           s            e           r
  
BASE64            [0111 01]    [0101 11]     [0011 01]    [10 0101]    +  [0111 00]  [10 0000] [0011 1101] [0011 1101]
十进制                29           23            13          37              28          32         =           =
                                   
base64[n]             d             X            N           l               c           g          =           =

结果   "dXNlcg=="

*/
/*

Base64 编码,以3个字节为1个字符进行编码；
如果数据长度不够，则补“=”

*/

//将string 转化为二进制
string s_to_2(const string& s)
{
	string resault;

	for (auto a : s)
	{
		// a为char 转为十进制Int
		unsigned int i = a;

		//int 转为 二进制，重复进行除二的操作，求余

		string temp;

		while (i > 0)
		{
			int t = i % 2;
			i = i / 2;
			if (t == 1) temp.insert(temp.begin(), '1');
			else temp.insert(temp.begin(), '0');
		}
		while (temp.size() < 8) temp.insert(temp.begin(), '0');

		//cout << temp << endl;
		resault += temp;
	}
	return resault;
}

char _2_6bit_to_s(const string & s)  //s为6位二进制 
{

	vector<int> num{ 1,2,4,8,16,32 };
	int u = 0;

	unsigned bit_s = 0;

	for (int i =s.size()-1;i >= 0;i--)
	{
		if (s[i] == '1') bit_s += num[u];
		u++;
	}

	return base64[bit_s];
}

char s_8bit_to_s(const string& s)   //将8个二进制转化为char
{

	vector<int> num{ 1,2,4,8,16,32,64,128 };
	int u = 0;

	unsigned bit_s = 0;

	for (int i = s.size() - 1; i >= 0; i--)
	{
		if (s[i] == '1') bit_s += num[u];
		u++;
	}
	return char(bit_s);
}

string pack_base64(const string& s)
{
	string s_2 = s_to_2(s); // 转化为二进制
	string resault;

	// 以3*8 个字节为一组进行处理，然后看剩下的是 2*8还是1*8 或者0*8
	int n = s_2.size() / 24;   // 共有 n 个3字节数组

	for (int i = 0; i < n; i++)
	{
		string temp(s_2,i*24, i*24 + 24);  //读取24个字符，进行 每6位 存一个结果，到string中；
		
		char a = _2_6bit_to_s(string(temp, 0, 6));
		char b = _2_6bit_to_s(string(temp, 6, 6));
		char c = _2_6bit_to_s(string(temp, 12,6 ));
		char d = _2_6bit_to_s(string(temp, 18,6));
		
		stringstream ss;
		ss << a << b << c << d;
		resault.append(ss.str());
	}

	if (s_2.size() % 24 == 16)  //  剩下16位的处理方法
	{
		string temp(s_2, s_2.size() - 16, 16);  //得到16位二进制
		temp.append("00");  //补满 18位置

		char a = _2_6bit_to_s(string(temp, 0, 6));
		char b = _2_6bit_to_s(string(temp, 6, 6));
		char c = _2_6bit_to_s(string(temp, 12,6));
		char d = '=';

		stringstream ss;
		ss << a << b << c << d;
		resault.append(ss.str());

	}
	else if (s_2.size() % 24 == 8) //剩下一个位置 8个字节的处理方法
	{
		string temp(s_2, s_2.size() - 8,8);  //得到 8 位二进制
		
		temp.append("0000");

		char a = _2_6bit_to_s(string(temp, 0, 6));
		char b = _2_6bit_to_s(string(temp, 6, 6));
		char c = '=';
		char d = '=';

		stringstream ss;
		ss << a << b << c << d;
		resault.append(ss.str());
	}

	return resault;

}

string unpack_base64(const string& s)
{
	char un_base64[256] = { 0 };

	int m = 0;
	for (int i = char('A'); i <= char('Z'); i++)  // 处理映射关系 i 65->90  
	{
		un_base64[i] = m;
		m++;
	}
		
	for (int i = char('a'); i <= char('z'); i++)
	{
		un_base64[i] = m;
		m++;
	}

	for (int i = char('0'); i <= char('9'); i++)
	{
		un_base64[i] = m;
		m++;
	}

	// ‘+’ = 43   ‘/’ = 47
	un_base64['+'] = 62;
	un_base64['/'] = 63;

	//un_base64 即把 64编码后的字符 转换为 十进制的数字
	//然后将十进制的数字 转换为2进制，取其中后6位


	string t = s;
	string temp;
	string resault;

	for (auto a : t)
	{
		if (a == '=') break;
		char a_2 = un_base64[a];  //获得 十进制对应的

		stringstream ss;
		ss << a_2;

		string s = s_to_2(ss.str());  //获取字符的二进制，并且抛弃头两位；

		temp.append(s,2,6);
	}

	// 然后将temp按照八位解码
	int n = temp.size() / 8;

	for (int i = 0; i < n; i++) //后续可能多余的0 直接可以无视 都是补码
	{
		char a = s_8bit_to_s(string(temp, i*8, 8));
		resault += a;
	}

	return resault;
}