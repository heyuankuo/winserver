#pragma once
#include "SockStack.h"

class CNetWorkManager
{
public:
	CNetWorkManager();
	~CNetWorkManager();

	// 外部调用接口
public:
	// 启动服务
	int startnetwork();

	// 停止服务
	void stopnetwork();

protected:

	// 发送信息，此函数保证信息发送完，或者报错
	static int SafeSend(SOCKET sock, const char *buffer, unsigned len);

	// 接收信息，此函数保证信息接收完成，或者报错
	static int SafeRecv(SOCKET sock, byte *buffer, int buffersize, int step);

	// 此函数用于接收客户端请求并给予相应应答
	static unsigned __stdcall recv_client(void *accp);

	// 此函数用于管理和配置客户连接
	static void MangerClient(void *pclient);

};

