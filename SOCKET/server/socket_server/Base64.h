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
ASCII  "user"   [0111 0101] [0111 0011] [0110 0101]  [0111 0010]              //  3ת4 + 1ת4  ��8*8 64�ֽ�
                     u           s            e           r
  
BASE64            [0111 01]    [0101 11]     [0011 01]    [10 0101]    +  [0111 00]  [10 0000] [0011 1101] [0011 1101]
ʮ����                29           23            13          37              28          32         =           =
                                   
base64[n]             d             X            N           l               c           g          =           =

���   "dXNlcg=="

*/
/*

Base64 ����,��3���ֽ�Ϊ1���ַ����б��룻
������ݳ��Ȳ������򲹡�=��

*/

//��string ת��Ϊ������
string s_to_2(const string& s)
{
	string resault;

	for (auto a : s)
	{
		// aΪchar תΪʮ����Int
		unsigned int i = a;

		//int תΪ �����ƣ��ظ����г����Ĳ���������

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

char _2_6bit_to_s(const string & s)  //sΪ6λ������ 
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

char s_8bit_to_s(const string& s)   //��8��������ת��Ϊchar
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
	string s_2 = s_to_2(s); // ת��Ϊ������
	string resault;

	// ��3*8 ���ֽ�Ϊһ����д���Ȼ��ʣ�µ��� 2*8����1*8 ����0*8
	int n = s_2.size() / 24;   // ���� n ��3�ֽ�����

	for (int i = 0; i < n; i++)
	{
		string temp(s_2,i*24, i*24 + 24);  //��ȡ24���ַ������� ÿ6λ ��һ���������string�У�
		
		char a = _2_6bit_to_s(string(temp, 0, 6));
		char b = _2_6bit_to_s(string(temp, 6, 6));
		char c = _2_6bit_to_s(string(temp, 12,6 ));
		char d = _2_6bit_to_s(string(temp, 18,6));
		
		stringstream ss;
		ss << a << b << c << d;
		resault.append(ss.str());
	}

	if (s_2.size() % 24 == 16)  //  ʣ��16λ�Ĵ�����
	{
		string temp(s_2, s_2.size() - 16, 16);  //�õ�16λ������
		temp.append("00");  //���� 18λ��

		char a = _2_6bit_to_s(string(temp, 0, 6));
		char b = _2_6bit_to_s(string(temp, 6, 6));
		char c = _2_6bit_to_s(string(temp, 12,6));
		char d = '=';

		stringstream ss;
		ss << a << b << c << d;
		resault.append(ss.str());

	}
	else if (s_2.size() % 24 == 8) //ʣ��һ��λ�� 8���ֽڵĴ�����
	{
		string temp(s_2, s_2.size() - 8,8);  //�õ� 8 λ������
		
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
	for (int i = char('A'); i <= char('Z'); i++)  // ����ӳ���ϵ i 65->90  
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

	// ��+�� = 43   ��/�� = 47
	un_base64['+'] = 62;
	un_base64['/'] = 63;

	//un_base64 ���� 64�������ַ� ת��Ϊ ʮ���Ƶ�����
	//Ȼ��ʮ���Ƶ����� ת��Ϊ2���ƣ�ȡ���к�6λ


	string t = s;
	string temp;
	string resault;

	for (auto a : t)
	{
		if (a == '=') break;
		char a_2 = un_base64[a];  //��� ʮ���ƶ�Ӧ��

		stringstream ss;
		ss << a_2;

		string s = s_to_2(ss.str());  //��ȡ�ַ��Ķ����ƣ���������ͷ��λ��

		temp.append(s,2,6);
	}

	// Ȼ��temp���հ�λ����
	int n = temp.size() / 8;

	for (int i = 0; i < n; i++) //�������ܶ����0 ֱ�ӿ������� ���ǲ���
	{
		char a = s_8bit_to_s(string(temp, i*8, 8));
		resault += a;
	}

	return resault;
}