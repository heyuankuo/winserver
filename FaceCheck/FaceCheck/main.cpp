//#include <windows.h>
#include <iostream>
#include "dbclass/SqliteAccess.h"
#include "NetWorkManager.h"

int main( int argc, char *argv[])
{
	// 加载数据库
	CSqliteAccess saq;
	saq.LoadData();

	// 启动服务
	CNetWorkManager netwk;
	netwk.startnetwork(); 

	return 0;
}