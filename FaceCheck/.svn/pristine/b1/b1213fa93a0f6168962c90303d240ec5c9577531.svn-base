//
// Cmpnt3.cpp - Component 3
//
#include <iostream>
#include <objbase.h>
#include "sqlite3.h"
#include "ichap07.h"
#include "CUnknown.h"
#include "A.H"

#pragma comment(lib, "sqlite3.lib")

const char *DBDIR = "E:\\faceDB.db";

static int callback(void *data, int argc, char **argv, char **azColName) 
{
	UserInfos *users = (UserInfos *)data;
	int i = users->sum_users;

	users->first_user[i] = new UserInfo[argc]();

	memcpy_s(users->first_user[i]->_name, sizeof users->first_user[i]->_name, argv[2], sizeof users->first_user[i]->_name);
		
	users->first_user[i]->_feats = new unsigned char[ 8 * 1024]();
	memcpy_s(users->first_user[i]->_feats, sizeof users->first_user[i]->_feats, argv[4], sizeof users->first_user[i]-> _feats);
	// MultiByteToWideChar(CP_UTF8, NULL, (LPCCH)users[i]->, -1, name, 5);

	users->sum_users += 1;
	return 0;
}

///////////////////////////////////////////////////////////
//
// Interface IFaceDBOS
//

/**
 * 加载数据
 * 将所有模板一次性加载到缓存中
 */
HRESULT __stdcall CA::LoadData()
{
	sqlite3 *db;
	int result = sqlite3_open(DBDIR, &db);

	sqlite3_stmt *stat;
	const char *pzTail = NULL;
	sqlite3_prepare(db, "select * from faces;", -1, &stat, &pzTail);
	// sqlite3_column_count(stat);	// 记录列数

	// 遍历
	result = sqlite3_step(stat);
	int i = 0;
	while (result == SQLITE_ROW) 
	{
		users.first_user[i] = new UserInfo();
		strcpy_s((char *)users.first_user[i]->_name,
			sizeof users.first_user[i]->_name,
			(char *)sqlite3_column_text(stat, 2));

		users.first_user[i]->_feats = new BYTE[1024 * 8]();
		memcpy_s(users.first_user[i]->_feats, 
			1024 *8,
			sqlite3_column_blob(stat, 4),
			1024 * 8);

		i++;
		result = sqlite3_step(stat);
	}
	users.sum_users = i;
	sqlite3_finalize(stat);

	sqlite3_close(db);
	return S_OK;
}

/**
 * 查询数据
 * 在缓存中查询数据
 */
HRESULT CA::QueryFace(UserInfos **users)
{
	*users = &this->users;
	return S_OK;
}

/**
 * 插入一条数据
 */
HRESULT __stdcall CA::InserData(UserInfo *user, int recoder_id)
{
	sqlite3 *db;
	int rc;

	 // 打开数据库
	rc = sqlite3_open(DBDIR, &db);
	if (rc) 
	{
		// 数据库打开失败
		return E_FAIL;
	}

	// 查询字符串模板
	const char *sql_format = "INSERT INTO faces (ID,userid,username,usergender,feat,login_times, period) "
		"VALUES (%d, %d, '%s', '%s', ?, '%s', %d ); ";

	// 构造查询字符串
	char sql_buffer[1024 * 16] = { 0 };
	sprintf_s( sql_buffer, sizeof sql_buffer, sql_format, recoder_id,
		user->_id, user->_name, user->_gender, "2017-6-12", 1);

	sqlite3_stmt *stat;
	const char *pzTail = NULL;
	rc = sqlite3_prepare(db, sql_buffer, -1, &stat, &pzTail);
	rc = sqlite3_bind_blob(stat, 1, user->_feats, user->_feat_len, NULL);
	rc = sqlite3_step(stat);
	sqlite3_finalize(stat);

	sqlite3_close(db);
	return S_OK;
}

//
// Constructor
//
CA::CA(IUnknown* pUnknownOuter)
: CUnknown(pUnknownOuter)
{
}

CA::~CA() 
{
}

//
// NondelegatingQueryInterface implementation
//
HRESULT __stdcall CA::NondelegatingQueryInterface(const IID& iid,
                                                  void** ppv)
{ 	
	if (iid == __uuidof(IFaceDBOS))
	{
		return FinishQI(static_cast<IFaceDBOS*>(this), ppv) ;
	}
	else
	{
		return CUnknown::NondelegatingQueryInterface(iid, ppv) ;
	}
}


///////////////////////////////////////////////////////////
//
// Creation function used by CFactory
//
HRESULT CA::CreateInstance(IUnknown* pUnknownOuter,
                           CUnknown** ppNewComponent) 
{
	*ppNewComponent = new CA(pUnknownOuter) ;
	return S_OK ;
}
