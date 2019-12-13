#pragma once
#include "Im_message.h"
#include <sstream>
#include <map>
#include <string>

using namespace std;


/*
	#include "Im_message.h"
	接受服务器的信息，并且按照约定标准进行相应处理
	1.接收消息  2.修改本地好友休息 3.查找账号的列表

	暂定用一个 char [1500] 来接收信息流；

	约定发送信息流的标准：

	开头int t;(type)

	if(t==1)
	发送内容为 user_mes,用来更新本地信息；
	后续连续4个int 分别为4个  string 的拷贝size；

	if(t==2)
	发送内容为 chat_mes，意思为聊天信息
	后续连续4个int 分别是4个 string的拷贝size长度

	if(t==3)
	发送的内容为Login_mes；
	后面会有两个int u_size,int off_size;

	首先按照标准读取 1+u_size+off_size次，
	每次读取的内容都是（t+结构体数据）

	if(t==4)
	发送的内容为好友列表
	后面会有一个int 表示里面有多少个数据包；
	每个包的结构是（t+结构体数据）

	if(t==5)
	发送内容为一个string，目的是获取vector<user_mes> 好友列表

	if(t==6)
	添加好友请求到服务器，内容为一个string;
	后面有一个int 表示string长度；
	string 内容为： 请求者ID 被添加者ID   例：  string s("5023444 13241234");

	if(t==7)
	删除好友请求到服务器，和添加一致；

	if(t==99)
	客户端发来的心跳检测；


	以下为文件部分：
	-------------------------------------------------------------------

	发送过程
	-------------------------------------------------------------
	if(t==50)
	发送的内容为一个文件,后面4个int ，然后后接 file_mes结构体；

	if(t==51)
	表示实际开始发送了文件内容，前4个int 表示struct file_mes的string 长度；
	然后后接一个int size 表示此次传输的文件大小，后接size个二进制数据；

	if(t==52) 
	表示接受端应答，收到了 该 file_mes的信息；
	发送端继续发送下一段；

	if(t==53)
	表示发送端文件发送完毕；
	接受端进行hash校验

	if(t==54)
	表示接受端接受完毕，没有问题；

	发送异常
	-------------------------------------------------------------
	if(t=55) 
	发送端中断文件发送，表示文件发送失败；

	if(t==56)
	接受端明白文件接受失败，释放相关资源；

	文件传输中，如果接受端没有给予反馈，那么发送端会重新尝试发送3次，连续失败后，中断文件发送；
	-------------------------------------------------------------

*/

pack_mes unpack_mes(char* source, unsigned& index)
{
	int size_1, size_2, size_3, size_4;
	char temp[4000] = { 0 };
	pack_mes u;

	//index += 4;//跳过头部type_t;

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
	// 从流中读解包
	// 解大包，得到好友列
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
	int size; //包的大小

	index += 4;
	memcpy(&size, source + index, 4);
	index += 4;

	char buffer[200] = { 0 };
	memcpy(&buffer, source + index, size);
	stringstream ss;
	ss << buffer;

	return ss.str();
}

//打包函数 打包结构体 和 打包的流
void pack_user(user_mes m,char* position)
{
	memset(position, 0, 4000);
	int t = 1;
	int size_1 = m.ID.size();
	int size_2 = m.NAME.size();
	int size_3 = m.SIG.size();
	int size_4 = m.PASSWORD.size();

	unsigned index = 0;

	memcpy(position + index, &t, sizeof(t)); // 类型为1
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

	memcpy(position + index, &t, sizeof(t)); // 类型为1
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