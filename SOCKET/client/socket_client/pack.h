#pragma once
#include "Im_message.h"
#include <sstream>
#include <map>
#include <string>

using namespace std;


/*
	#include "Im_message.h"
	���ܷ���������Ϣ�����Ұ���Լ����׼������Ӧ����
	1.������Ϣ  2.�޸ı��غ�����Ϣ 3.�����˺ŵ��б�

	�ݶ���һ�� char [1500] ��������Ϣ����

	Լ��������Ϣ���ı�׼��

	��ͷint t;(type)

	if(t==1)
	��������Ϊ user_mes,�������±�����Ϣ��
	��������4��int �ֱ�Ϊ4��  string �Ŀ���size��

	if(t==2)
	��������Ϊ chat_mes����˼Ϊ������Ϣ
	��������4��int �ֱ���4�� string�Ŀ���size����

	if(t==3)
	���͵�����ΪLogin_mes��
	�����������int u_size,int off_size;

	���Ȱ��ձ�׼��ȡ 1+u_size+off_size�Σ�
	ÿ�ζ�ȡ�����ݶ��ǣ�t+�ṹ�����ݣ�

	if(t==4)
	���͵�����Ϊ�����б�
	�������һ��int ��ʾ�����ж��ٸ����ݰ���
	ÿ�����Ľṹ�ǣ�t+�ṹ�����ݣ�

	if(t==5)
	��������Ϊһ��string��Ŀ���ǻ�ȡvector<user_mes> �����б�

	if(t==6)
	��Ӻ������󵽷�����������Ϊһ��string;
	������һ��int ��ʾstring���ȣ�
	string ����Ϊ�� ������ID �������ID   ����  string s("5023444 13241234");

	if(t==7)
	ɾ���������󵽷������������һ�£�

	if(t==99)
	�ͻ��˷�����������⣻


	����Ϊ�ļ����֣�
	-------------------------------------------------------------------

	���͹���
	-------------------------------------------------------------
	if(t==50)
	���͵�����Ϊһ���ļ�,����4��int ��Ȼ���� file_mes�ṹ�壻

	if(t==51)
	��ʾʵ�ʿ�ʼ�������ļ����ݣ�ǰ4��int ��ʾstruct file_mes��string ���ȣ�
	Ȼ����һ��int size ��ʾ�˴δ�����ļ���С�����size�����������ݣ�

	if(t==52) 
	��ʾ���ܶ�Ӧ���յ��� �� file_mes����Ϣ��
	���Ͷ˼���������һ�Σ�

	if(t==53)
	��ʾ���Ͷ��ļ�������ϣ�
	���ܶ˽���hashУ��

	if(t==54)
	��ʾ���ܶ˽�����ϣ�û�����⣻

	�����쳣
	-------------------------------------------------------------
	if(t=55) 
	���Ͷ��ж��ļ����ͣ���ʾ�ļ�����ʧ�ܣ�

	if(t==56)
	���ܶ������ļ�����ʧ�ܣ��ͷ������Դ��

	�ļ������У�������ܶ�û�и��跴������ô���Ͷ˻����³��Է���3�Σ�����ʧ�ܺ��ж��ļ����ͣ�
	-------------------------------------------------------------

*/

pack_mes unpack_mes(char* source, unsigned& index)
{
	int size_1, size_2, size_3, size_4;
	char temp[4000] = { 0 };
	pack_mes u;

	//index += 4;//����ͷ��type_t;

	memcpy(&size_1, source + index, 4);
	index += 4;
	memcpy(&size_2, source + index, 4);
	index += 4;
	memcpy(&size_3, source + index, 4);
	index += 4;
	memcpy(&size_4, source + index, 4);

	stringstream ss;

	memcpy(&temp, source + index, size_1);
	index += size_1;
	ss << temp;
	u.s1 = ss.str();
	ss.clear();
	memset(temp, 4000, 0);

	memcpy(&temp, source + index, size_2);
	index += size_2;
	ss << temp;
	u.s2 = ss.str();
	ss.clear();
	memset(temp, 4000, 0);

	memcpy(&temp, source + index, size_3);
	index += size_3;
	ss << temp;
	u.s3 = ss.str();
	ss.clear();
	memset(temp, 4000, 0);

	memcpy(&temp, source + index, size_4);
	index += size_4;
	ss << temp;
	u.s4 = ss.str();
	ss.clear();
	memset(temp, 4000, 0);

	return u;
}

vector<pack_mes> unpack_vector(char* source, unsigned & index)
{
	// �����ж����
	// �������õ�������
	int pack_sum;
	memcpy(&pack_sum, source + index, sizeof(int));
	index += sizeof(int);

	if (pack_sum == 0) return vector<pack_mes>();
	vector<pack_mes> v;

	for (int i = 0; i < pack_sum; i++)		
		v.push_back(unpack_mes(source, index));

	return v;
}


string unpack_string(char* source, unsigned& index)
{
	int size; //���Ĵ�С

	index += 4;
	memcpy(&size, source + index, 4);
	index += 4;

	char buffer[200] = { 0 };
	memcpy(&buffer, source + index, size);
	stringstream ss;
	ss << buffer;

	return ss.str();
}

//������� ����ṹ�� �� �������
void pack_user(user_mes m,char* position)
{
	memset(position, 0, 4000);
	int t = 1;
	int size_1 = m.ID.size();
	int size_2 = m.NAME.size();
	int size_3 = m.SIG.size();
	int size_4 = m.PASSWORD.size();

	unsigned index = 0;

	memcpy(position + index, &t, sizeof(t)); // ����Ϊ1
	index += 4;

	memcpy(position + index, &size_1, 4);
	index += 4;

	memcpy(position + index, &size_2, 4);
	index += 4;

	memcpy(position + index, &size_3, 4);
	index += 4;

	memcpy(position + index, &size_4, 4);
	index += 4;

	memcpy(position + index, m.ID.c_str(), size_1);
	index += size_1;

	memcpy(position + index, m.NAME.c_str(), size_2);
	index += size_2;

	memcpy(position + index, m.SIG.c_str(), size_3);
	index += size_3;

	memcpy(position + index, m.PASSWORD.c_str(), size_4);
	index += size_4;

	return;
}

void pack_chat(chat_mes m, char* position)
{
	memset(position, 0, 4000);
	int t = 2;
	int size_1 = m.sent_id.size();
	int size_2 = m.rec_id.size();
	int size_3 = m.chat_content.size();
	int size_4 = m.time.size();

	unsigned index = 0;

	memcpy(position + index, &t, sizeof(t)); // ����Ϊ1
	index += 4;

	memcpy(position + index, &size_1, 4);
	index += 4;

	memcpy(position + index, &size_2, 4);
	index += 4;

	memcpy(position + index, &size_3, 4);
	index += 4;

	memcpy(position + index, &size_4, 4);
	index += 4;

	memcpy(position + index, m.sent_id.c_str(), size_1);
	index += size_1;

	memcpy(position + index, m.rec_id.c_str(), size_2);
	index += size_2;

	memcpy(position + index, m.chat_content.c_str(), size_3);
	index += size_3;

	memcpy(position + index, m.time.c_str(), size_4);
	index += size_4;

	return;
}

void pack_string(int t,string s, char* position)
{
	memset(position, 0, 4000);

	int size = s.size();

	int index = 0;

	memcpy(position + index, &t, 4);
	index += 4;

	memcpy(position + index, &size, 4);
	index += 4;

	memcpy(position + index, s.c_str(), size);

	return;
}