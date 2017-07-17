#include <iostream>
#include <vector>
#include <WS2tcpip.h>
#include <process.h>
#include "winsock2.h"
#include "SockStack.h"
#include "FaceMessage.h"
#pragma comment(lib, "ws2_32.lib")

using std::cout;
using std::endl;
void trace(char *s)
{
	cout << s << endl;
}

/**
* 接收
* 此函数实现客户端数据接收
* 定长接收，避免沾包
* 注意，此函数中step 参数不能比缓冲区大。
* @param			sock			[in]		客户套接字描述符
* @param			buffer			[in,out]	接收缓冲区
* @param			buffersize		[in]		接收缓冲区大小
* @param			step			[in]		定长接收，每次接收的字节数
*/
int SafeRecv(SOCKET sock, byte *buffer, int buffersize, int step)
{
	// 接收状态标志
	// 正常时此标志为0
	// 发生异常时，此标志置-1，此时函数依旧会接收完所有用户数据
	// 以保证此次接收正常完成，但数据并不写入缓冲区
	// 函数结束时返回此标志值
	int _flag = 0;

	int recvsum = 0;	// 接收计数
	memset(buffer, 0, buffersize);
	while (1)
	{
		int err;
		if (0 == _flag)
		{
			err = recv(sock, (char *)(buffer + recvsum), step, 0);	// 正常接收
		}
		else if (-1 == _flag)
		{
			err = recv(sock, (char *)(buffer), buffersize, 0);		// 异常接收
		}
		if (SOCKET_ERROR == err)
		{
			err = GetLastError();
			if (10035 == err)
			{
				return _flag;
			}
			else
			{
				return -1;
			}
		}
		else if (err > 0)
		{
			// trace(buffer + recvsum);
			recvsum += err;	// 已经接受的字节数
			if (buffersize - recvsum < step)
			{
				// trace("数据包长度超出最大缓冲区， 接收异常");
				_flag = -1;
			}
			continue;
		}
	}
}

/**
 * 发送信息
 * 此函数确保所有信息发送完成
 */
int SafeSend( SOCKET sock, const char *buffer, unsigned len)
{
	unsigned tmplen = 0;
	while (tmplen < len )
	{
		int err = 0;
		err = send(sock, buffer + tmplen, len - tmplen, 0);
		if (SOCKET_ERROR == err)
		{
			if (10035 == err)
			{
				Sleep(10);
				continue;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			tmplen += err;
		}
		
	}

	return tmplen;
}

/**
 * 客户端发送程序
 */
unsigned _stdcall send_proc(void *accp)
{
	CAccepter *paccp = (CAccepter *)accp;
	CFaceMessage *msgparse = (CFaceMessage *)paccp->msgparse;
	while (1)
	{
		int nIndex = ::WSAWaitForMultipleEvents(1, paccp->m_pevent, TRUE, WSA_INFINITE, FALSE);
		if (nIndex == WSA_WAIT_FAILED)
		{
			// 异常消息处理
			trace("客户连接异常");
		}

		// 提取事件
		WSANETWORKEVENTS event = { 0 };
		::WSAEnumNetworkEvents(*(paccp->m_psock), *(paccp->m_pevent), &event);

		if (event.lNetworkEvents & FD_WRITE)		// 写 事件
		{
			if (event.iErrorCode[FD_WRITE_BIT] == 0)
			{
				// 创建识别XML报文
				/*msgparse->img_height = 100;
				msgparse->img_width = 100;
				msgparse->m_flag1 = 21;
				const char *imgpath = "110.jpg";
				memcpy_s(msgparse->imgname, sizeof msgparse->imgname, imgpath, strlen(imgpath) + 1);*/
				
				// 创建添加用户报文
				/*msgparse->img_height = 100;
				msgparse->img_width = 100;
				msgparse->m_flag1 = 1;
				const char *imgpath = "110.jpg";
				memcpy_s(msgparse->imgname, sizeof msgparse->imgname, imgpath, strlen(imgpath) + 1);*/

				// 创建删除用户报文
				/*msgparse->m_flag1 = 11;
				msgparse->m_userid = 3;*/	

				// 创建修改用户报文
				msgparse->m_flag1 = 31;
				msgparse->m_userid = 5;
				const char *username = "110";
				memcpy_s(msgparse->m_username, sizeof msgparse->m_username, username, strlen(username) + 1);

				// 创建报文并发送
				memset(paccp->m_buffer_send, 0, SEND_BUFFER_SIZE);
				msgparse->msgcreate((char *)paccp->m_buffer_send, SEND_BUFFER_SIZE);
				int a = SafeSend( *(paccp->m_psock), (char *)paccp->m_buffer_send, 32 + strlen(msgparse->str_face_xml) + msgparse->size_image_base64);
				// cout << "结果为：" << a << endl;
			}
		}
		else if (event.lNetworkEvents & FD_READ)		// 度事件
		{
			if (event.iErrorCode[FD_READ_BIT] == 0)
			{
				// 接收报文
				Sleep(10);
				int err = SafeRecv(*(paccp->m_psock), paccp->m_buffer_recv, RECV_BUFFER_SIZE, 1024 * 8);
				if (0 == err)
				{
					// 报文处理
					msgparse->msgparse((char *)paccp->m_buffer_recv, (char *)paccp->m_buffer_send, RECV_BUFFER_SIZE);
					msgparse->msgproc();
					// msgparse->msgcreate((char *)paccp->m_buffer_send, SEND_BUFFER_SIZE);
				}
			}
		}
	}
	
}

int main( int argc, char *argv[])
{

	// 套接字初始化
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		trace("WSAStartup failed!");
		return 1;
	}

	// 创建套接字
	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock_server)
	{
		trace("socket failed!");
		WSACleanup();//释放套接字资源;  
		return  -1;
	}

	// 指定工作方式
	WSAEVENT event_server = ::WSACreateEvent();
	int err = WSAEventSelect(sock_server, event_server, FD_READ | FD_WRITE);
	if (SOCKET_ERROR == err)
	{
		trace("异步配置失败");
		closesocket(sock_server);
		WSACleanup();
		return -1;
	}

	// 服务器套接字地址配置
	SOCKADDR_IN		addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(6000);
	addr_server.sin_addr.s_addr = inet_addr("192.168.1.168");

	CFaceMessage msgparseobj; // 报文解析对象

	CAccepter accp;
	accp.m_psock = &sock_server;
	accp.m_pevent = &event_server;
	accp.m_buffer_send = new byte[SEND_BUFFER_SIZE]();
	accp.m_buffer_recv = new byte[RECV_BUFFER_SIZE]();
	accp.msgparse = (void *)&msgparseobj;

	err = connect(sock_server, (sockaddr *)&addr_server, sizeof addr_server);
	if (SOCKET_ERROR == err)
	{
		err = GetLastError();
		if (10035 == err)
		{

		}
		else
		{
			trace("连接失败");
		}
		
	}

	HANDLE h_sendThread = (HANDLE)_beginthreadex(NULL, NULL, send_proc, (void *)&accp, 0, NULL);

	::WaitForSingleObject(h_sendThread, INFINITE);
	CloseHandle(h_sendThread );
	return 0;
}