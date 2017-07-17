#include "SockStack.h"
#include <iostream>

using std::cout;
using std::endl;

CSockStack::~CSockStack()
{
	DeleteCriticalSection(&gSection);
}

/**
* 添加套接字-事件
* @param		sock		[in]		套接字描述
* @param		event		[in]		事件句柄
* @return		1		表示项目已达上限
* @return		0		表示操作成功
*/
int CSockStack::AddRecord(SOCKET sock, WSAEVENT event)
{
	if (WSA_MAXIMUM_WAIT_EVENTS == m_uindex)
	{
		// 栈已装满
		return 1;
	}

	EnterCriticalSection(&gSection);

	m_socks[m_uindex] = sock;
	m_events[m_uindex] = event;
	InterlockedIncrement(&m_uindex);

	cout << "客户端数量：" << m_uindex << endl;

	LeaveCriticalSection(&gSection);
	return 0;
}

/**
* 删除套接字-事件
* 此函数只管理套接字队列，并不管理套接字连接
* 删除套接字时，将套接字和其事件从队列中剔除，并不关闭套接字连接
* 注：套接字连接关闭通常在客户连接管理中关闭和释放
* @param		index		[in]		要删除的项目的索引
*/
int CSockStack::DeleteRecord(unsigned index)
{
	if( index <0 || index > m_uindex - 1)
	{ 
		// 输入参数无效
		return 1;
	}
	EnterCriticalSection(&gSection);
	
	for (unsigned i = index; i < m_uindex; ++i)
	{
		m_socks[i] = m_socks[i + 1];
		m_events[i] = m_events[i + 1];
	}

	m_socks[m_uindex] = 0;
	m_events[m_uindex] = 0;
	InterlockedDecrement(&m_uindex);

	cout << "客户端数量：" << m_uindex << endl;

	LeaveCriticalSection(&gSection);

	return 0;
}
