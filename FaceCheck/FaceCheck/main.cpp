//#include <windows.h>
#include <iostream>
#include "dbclass/SqliteAccess.h"
#include "NetWorkManager.h"

int main( int argc, char *argv[])
{
	// �������ݿ�
	CSqliteAccess saq;
	saq.LoadData();

	// ��������
	CNetWorkManager netwk;
	netwk.startnetwork(); 

	return 0;
}