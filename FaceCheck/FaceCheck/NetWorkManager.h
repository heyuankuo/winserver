#pragma once
#include "SockStack.h"

class CNetWorkManager
{
public:
	CNetWorkManager();
	~CNetWorkManager();

	// �ⲿ���ýӿ�
public:
	// ��������
	int startnetwork();

	// ֹͣ����
	void stopnetwork();

protected:

	// ������Ϣ���˺�����֤��Ϣ�����꣬���߱���
	static int SafeSend(SOCKET sock, const char *buffer, unsigned len);

	// ������Ϣ���˺�����֤��Ϣ������ɣ����߱���
	static int SafeRecv(SOCKET sock, byte *buffer, int buffersize, int step);

	// �˺������ڽ��տͻ������󲢸�����ӦӦ��
	static unsigned __stdcall recv_client(void *accp);

	// �˺������ڹ��������ÿͻ�����
	static void MangerClient(void *pclient);

};
