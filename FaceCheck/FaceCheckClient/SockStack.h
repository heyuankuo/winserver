/**
 * 套接字管理类
 * 此类负责管理套接字队列
 */
#pragma once
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

#define RECV_BUFFER_SIZE		1024 * 1024 * 3
#define SEND_BUFFER_SIZE		1024 * 1024 * 2

 /**
 * 此结构于包装客户对象
 * 从而在接收和发送线程操作套接字
 */
struct CAccepter
{
	SOCKET		*m_psock;			// 客户套接字
	WSAEVENT	*m_pevent;			// 套接字事件
	SOCKADDR_IN m_addr;				// 客户机地址
	HANDLE		m_hthread_recv;		// 接收句柄
	HANDLE		m_hthread_send;		// 发送句柄
	byte		*m_buffer_recv;		// 接收数据缓存
	byte		*m_buffer_send;		// 发送数据缓存

	void		*msgparse;			// 报文解析器
};

class CSockStack
{
public:
	CSockStack() :m_uindex(0)
	{
		memset(&m_events, 0, sizeof m_events);
		memset(&m_socks, 0, sizeof m_socks);
		InitializeCriticalSection(&gSection);
	}
	~CSockStack();

public:
	/**
	 * 添加套接字-事件对到
	 * @param		sock		[in]		套接字描述
	 * @param		event		[in]		事件句柄
	 */
	int AddRecord( SOCKET sock, WSAEVENT event);

	/**
	 * 删除套接字-事件
	 * @param		index		[in]		要删除的项目的索引
	 */
	int DeleteRecord( unsigned index);

public:

	SOCKET		m_sock_server;		// 服务器套接字

	SOCKET      m_socks[WSA_MAXIMUM_WAIT_EVENTS];		// 客户连接套接字
	WSAEVENT    m_events[WSA_MAXIMUM_WAIT_EVENTS];		// 客户连接套接字注册事件，与服务套接字一一对应
	
	ULONG		m_uindex; // 栈顶

	CRITICAL_SECTION gSection;
};

