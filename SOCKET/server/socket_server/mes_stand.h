#pragma once

#define USER_MES 1
#define CHAT_MES 2
#define LOGIN_MES 3
#define FRIEND_LIST 4
#define CHAT_LIST 9

#define MODIFY_MES 10

#define FIND_USER 5
#define ADD_FRI_REQUEST 6
#define ADD_FRI_RESAULT 7
#define DEL_FRI_REQUEST 11

#define LOGIN_ERROR 8

#define HEARTING 99

#define SYSTEM_ADMIN "1000000"

#define MAX_STRING_SIZE 4000
#define MAX_STREAM_SIZE 10000

#define PORT 5000
#define SERVER_IP "127.0.0.1"


/*

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
