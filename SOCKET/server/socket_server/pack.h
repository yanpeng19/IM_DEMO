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

void pack_user(user_mes& m, char* position, unsigned& index);

/*
��� chat_mes �ṹ��,����Ϊ 4�� string
-���ͷ����ʶ  chat_mes 1/user_mes 2
-��ȡ4��int ���� size_1,size_2,size_3,size_4
-���ݳ��� ���4��string
*/
pack_mes unpack_mes(char* source, unsigned& index)
{
	index += sizeof(int);                      //  ��ʱ����ͷ����ʶ����Ϊ1/2�ṹͨ��
	int size_1, size_2, size_3, size_4;   

	char temp[MAX_STRING_SIZE] = { 0 };        //  string ��������Ϊ4000��ʱ
	pack_mes u;

	memcpy(&size_1, source + index, 4);        //  ���4��string����             
	index += sizeof(int);
	memcpy(&size_2, source + index, 4);
	index += sizeof(int);
	memcpy(&size_3, source + index, 4);
	index += sizeof(int);
	memcpy(&size_4, source + index, 4);
	index += sizeof(int);

	stringstream ss;

	memcpy(&temp, source + index, size_1);     //  ����4��string���ݵ��ַ�������
	index += size_1;
	ss << temp;
	u.s1 = ss.str();
	ss.str("");
	memset(temp, 0,4000);                     

	memcpy(&temp, source + index, size_2);  
	index += size_2;
	ss << temp;
	u.s2 = ss.str();
	ss.str("");
	memset(temp, 0, 4000);

	memcpy(&temp, source + index, size_3);
	index += size_3;
	ss << temp;
	u.s3 = ss.str();
	ss.str("");
	memset(temp, 0, 4000);

	memcpy(&temp, source + index, size_4);
	index += size_4;
	ss << temp;
	u.s4 = ss.str();
	ss.str("");
	memset(temp, 0, 4000);

	return u;
}

/*
���һ��VECTOR<user/chat>������

-���ͷ����ʶ
-���vector����
-��������ṹ��

*/
void pack_vector(vector<user_mes> v,char* source, unsigned& index)
{
	int t = FRIEND_LIST;

	memcpy(source, &t, sizeof(int));
	index += sizeof(int);

	int size = v.size();
	for (auto a : v)
	{
		pack_user(a, source, index);
	}
		
	return;
}

/*
�����н��һ��vector<pack_mes>

-ͷ����ʶ friend_list 5/ chat_list 9
-���vector.size()
-����size,�������� unpack_mes �������Խ��������ݽ��

*/
vector<pack_mes> unpack_vector(char* source, unsigned & index)
{
	index += sizeof(int);                             // ��ʱ����ͷ��ʶ����Ϊ����ͨ��������������
	int pack_sum=0;                                   
	memcpy(&pack_sum, source + index, sizeof(int));   // ��� vector ����
	index += sizeof(int);                       

	vector<pack_mes> v;              

	for (int i = 0; i < pack_sum; i++)		          // ʹ�� unpack_mes ѭ���������
		v.push_back(unpack_mes(source, index));

	return v;
}

//string unpack_string(char* source, unsigned& index)
//{
//	int size; //���Ĵ�С
//
//	index += 4;
//	memcpy(&size, source + index, 4);
//	index += 4;
//
//	char buffer[200] = { 0 };
//	memcpy(&buffer, source + index, size);
//	stringstream ss;
//	ss << buffer;
//
//	return ss.str();
//}


/*
��user_mes�ṹ����������
���Ұ����ܳ��ȴ浽 couter��

-�����ʶ int(1)
-���4�� string�ĳ���
-���4�� string������

��index��Ϊ�����ã�����
*/
void pack_user(user_mes &m,char* position,unsigned & index)
{
	int t = 1;
	int size_1 = m.ID.size();
	int size_2 = m.NAME.size();
	int size_3 = m.SIG.size();
	int size_4 = m.PASSWORD.size();


	memcpy(position + index, &t, sizeof(t));         // ���ͷ����ʶint(1)
	index += sizeof(int);

	memcpy(position + index, &size_1, sizeof(int));  // ���4��string����
	index += sizeof(int);

	memcpy(position + index, &size_2, sizeof(int));
	index += sizeof(int);

	memcpy(position + index, &size_3, sizeof(int));
	index += sizeof(int);

	memcpy(position + index, &size_4, sizeof(int));
	index += sizeof(int);

	memcpy(position + index, m.ID.c_str(), size_1);  // ���ʵ������
	index += size_1;

	memcpy(position + index, m.NAME.c_str(), size_2);
	index += size_2;

	memcpy(position + index, m.SIG.c_str(), size_3);
	index += size_3;

	memcpy(position + index, m.PASSWORD.c_str(), size_4);
	index += size_4;

	return;
}

/*
��user_mes�ṹ����������
���Ұ����ܳ��ȴ浽 couter��

-�����ʶ int(2)
-���4�� string�ĳ���
-���4�� string������

��index��Ϊ�����ã�����
*/
void pack_chat(chat_mes m, char* position,unsigned & index)
{
	int t = 2;
	int size_1 = m.sent_id.size();
	int size_2 = m.rec_id.size();
	int size_3 = m.chat_content.size();
	int size_4 = m.time.size();

	memcpy(position + index, &t, sizeof(t));             // ���ͷ����ʶint(2)
	index += sizeof(t);

	memcpy(position + index, &size_1, sizeof(t));        // ���4��string����
	index += sizeof(t);

	memcpy(position + index, &size_2, sizeof(t));
	index += sizeof(t);

	memcpy(position + index, &size_3, sizeof(t));
	index += sizeof(t);

	memcpy(position + index, &size_4, sizeof(t));
	index += sizeof(t);

	memcpy(position + index, m.sent_id.c_str(), size_1);  // ���ʵ������
	index += size_1;

	memcpy(position + index, m.rec_id.c_str(), size_2);
	index += size_2;

	memcpy(position + index, m.chat_content.c_str(), size_3);
	index += size_3;

	memcpy(position + index, m.time.c_str(), size_4);
	index += size_4;

	return;
}

//void pack_string(int t,string s, char* position)
//{
//	memset(position, 0, 4000);
//
//	int size = s.size();
//
//	int index = 0;
//
//	memcpy(position + index, &t, 4);
//	index += 4;
//
//	memcpy(position + index, &size, 4);
//	index += 4;
//
//	memcpy(position + index, s.c_str(), size);
//
//	return;
//}

/*
Login_mes �ṹ�������������ṹ��ת������
�������ĳ��ȴ�Ϊ counter

-���   ��ͷ��ʾ int(3)
-���   login.user
-���   vector<string> friends ��size
-���   friends ������
-���   vector<mes_chat> off_line_mes size
-���   off_line_mes������

*/
void pack_login_mes(Login_mes login,char * position)
{
	unsigned index = 0;
	int t = 3;
	memcpy(position + index, &t, sizeof(int));            // �����ͷ 3
	index += sizeof(int);

	pack_user(login.user, position, index);               // ���user index �Զ�����
	
	t = 4;
	memcpy(position + index, &t, sizeof(int));            // ���vector��ʶ
	index += sizeof(int);

	int size = login.friends.size();           
	memcpy(position + index, &size, sizeof(int));         // ����friends����
	index += sizeof(int);

	if (size != 0)                                        // ѭ�����friends������
	{
		for (int i = 0; i < size; i++)
			pack_user(login.friends[i], position, index);
	}
	
	t = 9;
	memcpy(position + index, &t, sizeof(int));            // ���vector��ʶ
	index += sizeof(int);

	size = login.off_line_mes.size();
	memcpy(position + index, &size, sizeof(int));         // ��� off_line ����
	index += 4;
	
	if (size != 0)                                        // ѭ����� off_line����
	{
		for (int i = 0; i < size; i++)
			pack_chat(login.off_line_mes[i], position, index);
	}

	return;
}

/*
��� ���� Login_mes 

-���   ��ͷ��ʾ int(3)
-���   login.user
-���   vector<string> friends ��size
-���   friends ������
-���   vector<mes_chat> off_line_mes size
-���   off_line_mes������

*/
Login_mes unpack_login_mes(char* position)    
{
	unsigned index = 4;                                      // ������ͷ
	Login_mes resault;

	resault.user = unpack_mes(position, index);              // ��� user ���ݣ�index �������Զ�����

	vector<pack_mes> temp = unpack_vector(position, index);  // ��� friends ���ݣ�index ��ӡ���Զ�����
	if (temp[0].s1 != "")                                    // ��������ݣ���ô��ȡ�ṹ������
	{
		for (auto a : temp)
			resault.friends.push_back(a);
	}

	temp = unpack_vector(position, index);                   // ��� off_line ����, index ����������
	if (temp[0].s1 != "")                                    // ��������ݣ���ô��ȡ�ṹ������
	{
		for (auto a : temp)
			resault.off_line_mes.push_back(a);
	}

	return resault;
}