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
* ����
* �˺���ʵ�ֿͻ������ݽ���
* �������գ�����մ��
* ע�⣬�˺�����step �������ܱȻ�������
* @param			sock			[in]		�ͻ��׽���������
* @param			buffer			[in,out]	���ջ�����
* @param			buffersize		[in]		���ջ�������С
* @param			step			[in]		�������գ�ÿ�ν��յ��ֽ���
*/
int SafeRecv(SOCKET sock, byte *buffer, int buffersize, int step)
{
	// ����״̬��־
	// ����ʱ�˱�־Ϊ0
	// �����쳣ʱ���˱�־��-1����ʱ�������ɻ�����������û�����
	// �Ա�֤�˴ν���������ɣ������ݲ���д�뻺����
	// ��������ʱ���ش˱�־ֵ
	int _flag = 0;

	int recvsum = 0;	// ���ռ���
	memset(buffer, 0, buffersize);
	while (1)
	{
		int err;
		if (0 == _flag)
		{
			err = recv(sock, (char *)(buffer + recvsum), step, 0);	// ��������
		}
		else if (-1 == _flag)
		{
			err = recv(sock, (char *)(buffer), buffersize, 0);		// �쳣����
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
			recvsum += err;	// �Ѿ����ܵ��ֽ���
			if (buffersize - recvsum < step)
			{
				// trace("���ݰ����ȳ�����󻺳����� �����쳣");
				_flag = -1;
			}
			continue;
		}
	}
}

/**
 * ������Ϣ
 * �˺���ȷ��������Ϣ�������
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
 * �ͻ��˷��ͳ���
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
			// �쳣��Ϣ����
			trace("�ͻ������쳣");
		}

		// ��ȡ�¼�
		WSANETWORKEVENTS event = { 0 };
		::WSAEnumNetworkEvents(*(paccp->m_psock), *(paccp->m_pevent), &event);

		if (event.lNetworkEvents & FD_WRITE)		// д �¼�
		{
			if (event.iErrorCode[FD_WRITE_BIT] == 0)
			{
				// ����ʶ��XML����
				/*msgparse->img_height = 100;
				msgparse->img_width = 100;
				msgparse->m_flag1 = 21;
				const char *imgpath = "110.jpg";
				memcpy_s(msgparse->imgname, sizeof msgparse->imgname, imgpath, strlen(imgpath) + 1);*/
				
				// ��������û�����
				/*msgparse->img_height = 100;
				msgparse->img_width = 100;
				msgparse->m_flag1 = 1;
				const char *imgpath = "110.jpg";
				memcpy_s(msgparse->imgname, sizeof msgparse->imgname, imgpath, strlen(imgpath) + 1);*/

				// ����ɾ���û�����
				/*msgparse->m_flag1 = 11;
				msgparse->m_userid = 3;*/	

				// �����޸��û�����
				msgparse->m_flag1 = 31;
				msgparse->m_userid = 5;
				const char *username = "110";
				memcpy_s(msgparse->m_username, sizeof msgparse->m_username, username, strlen(username) + 1);

				// �������Ĳ�����
				memset(paccp->m_buffer_send, 0, SEND_BUFFER_SIZE);
				msgparse->msgcreate((char *)paccp->m_buffer_send, SEND_BUFFER_SIZE);
				int a = SafeSend( *(paccp->m_psock), (char *)paccp->m_buffer_send, 32 + strlen(msgparse->str_face_xml) + msgparse->size_image_base64);
				// cout << "���Ϊ��" << a << endl;
			}
		}
		else if (event.lNetworkEvents & FD_READ)		// ���¼�
		{
			if (event.iErrorCode[FD_READ_BIT] == 0)
			{
				// ���ձ���
				Sleep(10);
				int err = SafeRecv(*(paccp->m_psock), paccp->m_buffer_recv, RECV_BUFFER_SIZE, 1024 * 8);
				if (0 == err)
				{
					// ���Ĵ���
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

	// �׽��ֳ�ʼ��
	WSADATA			wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		trace("WSAStartup failed!");
		return 1;
	}

	// �����׽���
	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock_server)
	{
		trace("socket failed!");
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	// ָ��������ʽ
	WSAEVENT event_server = ::WSACreateEvent();
	int err = WSAEventSelect(sock_server, event_server, FD_READ | FD_WRITE);
	if (SOCKET_ERROR == err)
	{
		trace("�첽����ʧ��");
		closesocket(sock_server);
		WSACleanup();
		return -1;
	}

	// �������׽��ֵ�ַ����
	SOCKADDR_IN		addr_server;
	addr_server.sin_family = AF_INET;
	addr_server.sin_port = htons(6000);
	addr_server.sin_addr.s_addr = inet_addr("192.168.1.168");

	CFaceMessage msgparseobj; // ���Ľ�������

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
			trace("����ʧ��");
		}
		
	}

	HANDLE h_sendThread = (HANDLE)_beginthreadex(NULL, NULL, send_proc, (void *)&accp, 0, NULL);

	::WaitForSingleObject(h_sendThread, INFINITE);
	CloseHandle(h_sendThread );
	return 0;
}