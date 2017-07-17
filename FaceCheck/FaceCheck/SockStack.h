/**
 * �׽��ֹ�����
 * ���ฺ������׽��ֶ���
 */
#pragma once
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

#define RECV_BUFFER_SIZE		1024 * 1024 * 3
#define SEND_BUFFER_SIZE		1024 * 1024 * 2

 /**
 * �˽ṹ�ڰ�װ�ͻ�����
 * �Ӷ��ڽ��պͷ����̲߳����׽���
 */
struct CAccepter
{
	SOCKET		*m_psock;			// �ͻ��׽���
	WSAEVENT	*m_pevent;			// �׽����¼�
	SOCKADDR_IN m_addr;				// �ͻ�����ַ
	HANDLE		m_hthread_recv;		// ���վ��
	HANDLE		m_hthread_send;		// ���;��
	byte		*m_buffer_recv;		// �������ݻ���
	byte		*m_buffer_send;		// �������ݻ���

	void		*msgparse;			// ���Ľ�����
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
	 * ����׽���-�¼��Ե�
	 * @param		sock		[in]		�׽�������
	 * @param		event		[in]		�¼����
	 */
	int AddRecord( SOCKET sock, WSAEVENT event);

	/**
	 * ɾ���׽���-�¼�
	 * @param		index		[in]		Ҫɾ������Ŀ������
	 */
	int DeleteRecord( unsigned index);

public:

	SOCKET		m_sock_server;		// �������׽���

	SOCKET      m_socks[WSA_MAXIMUM_WAIT_EVENTS];		// �ͻ������׽���
	WSAEVENT    m_events[WSA_MAXIMUM_WAIT_EVENTS];		// �ͻ������׽���ע���¼���������׽���һһ��Ӧ
	
	ULONG		m_uindex; // ջ��

	CRITICAL_SECTION gSection;
};

