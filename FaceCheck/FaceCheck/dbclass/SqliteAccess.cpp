#include "SqliteAccess.h"
#include "sqlite3.h"
#include "../SeetaFace.h"
#include <Windows.h>
#pragma comment(lib, "dbclass/sqlite3.lib")

const char *DBDIR = "E:\\faceDB.db";

list<USER_INFO *>user_memy_db; // 内存数据库， 全局唯一


// 记录日志
#include <time.h>
void trace( char *s)
{
	time_t tt = time(NULL);
	tm t;
	localtime_s(&t, &tt);
	printf("%d-%02d-%02d %02d:%02d:%02d\n",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec);

	std::cout << s << std::endl;
}

int CSqliteAccess::MAX_USERID = 0;

CSqliteAccess::CSqliteAccess()
{
}


CSqliteAccess::~CSqliteAccess()
{
}

/**
* 加载数据库数据
* 此函数在程序开始时加载所有数据库数据到内存中。
* 此函数返回最大OID
* @param			maxuserid			[in, out]		返回最大用户ID
*/
void CSqliteAccess::LoadData(  )
{
	// 清空内存数据集
	list<USER_INFO *>::iterator it_user_info;
	list<FACE_MODEL *>::iterator it_face_mod;
	for (it_user_info = user_memy_db.begin(); it_user_info != user_memy_db.end(); ++it_user_info)
	{
		for (it_face_mod = (*it_user_info)->faces.begin(); it_face_mod != (*it_user_info)->faces.end(); ++it_face_mod)
			delete (*it_face_mod);

		(*it_user_info)->faces.clear();

		delete(*it_user_info);
	}
	user_memy_db.clear();

	// 打开数据库
	sqlite3 *db = NULL;
	int result = sqlite3_open(DBDIR, &db);

	// 获取所有数据
	sqlite3_stmt *stat = NULL;
	const char *pzTail = NULL;
	sqlite3_prepare(db, "select * from faces;", -1, &stat, &pzTail);
	// sqlite3_column_count(stat);	// 记录列数

	MAX_USERID = 0;
	result = sqlite3_step(stat);

	// 遍历所有数据
	while (result == SQLITE_ROW)
	{
		// 查询内存数据集中是否已有当前用户
		int current_userid = sqlite3_column_int(stat, 1); // 获取当前userid
		MAX_USERID = (MAX_USERID > current_userid) ? MAX_USERID : current_userid;

		// 加载用户数据
		GetUserByUserid(current_userid, &it_user_info);	
		if (user_memy_db.end() != it_user_info)
		{
			(*it_user_info)->faces.push_back(new FACE_MODEL((char *)sqlite3_column_blob(stat, 4)));

		} // 已经有当前用户数据
		else
		{
			// 创建新用户
			USER_INFO *puser = new USER_INFO();

			puser->OID = sqlite3_column_int(stat, 1); // userid

			memset(puser->user_name, 0, sizeof puser->user_name);
			strcpy_s(puser->user_name, sizeof puser->user_name,
				(char *)sqlite3_column_text(stat, 2));		// username

			memset(puser->user_gender, 0, sizeof puser->user_gender);
			strcpy_s(puser->user_gender, sizeof puser->user_gender,
				(char *)sqlite3_column_text(stat, 3));		// usergender

			puser->faces.push_back(new FACE_MODEL((char *)sqlite3_column_blob(stat, 4)));
			user_memy_db.push_back(puser);

		}// 还没有当前用户数据

		result = sqlite3_step(stat);
	}

	sqlite3_finalize(stat);
	sqlite3_close(db);
	return ;
}

/**
 * 使用userid 查询用户数据
 * @pararm		userid		[in]		
 * @pararm		it_user		[out]		需要返回的迭代器
 */
int CSqliteAccess::GetUserByUserid(int userid, list<USER_INFO *>::iterator *it_user)
{
	if (NULL == it_user)
	{
		trace("使用userid查询用户数据出错，原因是输入的迭代器指针为空");
		return -1;
	}

	(*it_user) = user_memy_db.begin();
	for (; (*it_user) != user_memy_db.end(); (*it_user)++)
	{
		if (userid == (**it_user)->OID)
			break;
	}

	return 0;
}

/**
* 使用facemod 查询用户数据
* @pararm		img_file	[in]		照片路径
* @pararm		it_user		[out]		需要返回的迭代器
*/
int CSqliteAccess::GetUserByFace(char *img_file, list<USER_INFO *>::iterator *it_user)
{
	if (NULL == img_file)
	{
		trace("输入的路径为空");
		return -1;
	}

	// 提取人脸特征
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(img_file, feat);

	float result = 0.0f; // 单次判断结果缓存；
	list<USER_INFO *>::iterator it_index = user_memy_db.begin(); // 用于遍历数据库的迭代器

	// 开始比对
	// (*it_user) = 
	for (; it_index != user_memy_db.end(); it_index++)
	{
		for( list<FACE_MODEL *>::iterator it_face_mod = (*it_index)->faces.begin();
			it_face_mod != (*it_index)->faces.end(); it_face_mod++)
		{
			float match_tmp = sf.FeatureCompare( (float *)(*it_face_mod)->face_feature, feat); // 计算匹配度

			if (0.8f < match_tmp)
			{
				result = match_tmp;
				*it_user = it_index;
				break;			// 若检测到0.8 以上匹配，直接命中
			}

			if (0.60f > match_tmp)
				continue;		// 匹配在0.6 以下，直接跳过

			// 在0.8-0.6 之间，选择匹配度高的
			if (result < match_tmp)
			{
				result = match_tmp;
				*it_user = it_index;
			}

		}
	}
 	return 0;
}

/************************************************************************/
/* 添加新用户																*/
/************************************************************************/
int CSqliteAccess::AddUser(USER_INFO *user, char *imgname)
{
	if (NULL == user)
	{
		trace("添加新用户失败， 原因是输入参数为空");
		return -1;
	}

	// 提取人脸特征
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(imgname, feat);

	InterlockedIncrement((LONG *)&MAX_USERID); // 最大用户ID自增

	// 构造用户信息
	USER_INFO *puser = new USER_INFO();
	puser->OID = MAX_USERID;
	memcpy_s(puser->user_name, sizeof puser->user_name, user->user_name, sizeof puser->user_name);
	memcpy_s(puser->user_gender, sizeof puser->user_gender, user->user_gender, sizeof puser->user_gender);
	puser->user_age = user->user_age;

	// 添加模板
	puser->faces.push_back(new FACE_MODEL((char *)feat));

	user_memy_db.push_back(puser);

	updatedb(); // 更新数据库

	return 0;

}

/************************************************************************/
/* 给已有用户添加人脸模板                                                           */
/************************************************************************/
int CSqliteAccess::AddFaceMod(int userid, char *imgname)
{
	// 提取人脸特征
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(imgname, feat);

	// 查询指定的用户
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);	

	if (user_memy_db.end() != it_user_info)
	{
		(*it_user_info)->faces.push_back(new FACE_MODEL((char *)feat));
		return 0;
	}
	else
	{
		// 指定的用户不存在
		trace("添加模板失败，指定的用户不存在");
		return -1;
	}

	updatedb(); // 更新数据库
}

// 修改用户数据
int CSqliteAccess::ModifyUseInfo(int userid, char *username, char *usergender, int userage)
{
	// 查询指定的用户
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);
	
	if (user_memy_db.end() != it_user_info)
	{
		if (NULL != username)
			memcpy_s((*it_user_info)->user_name, sizeof(*it_user_info)->user_name, 
				username, sizeof(*it_user_info)->user_name);

		if (NULL != usergender)
			memcpy_s((*it_user_info)->user_gender, sizeof(*it_user_info)->user_gender,
				usergender, sizeof(*it_user_info)->user_gender);
		
		if (NULL != userage)
			(*it_user_info)->user_age = userage;

		updatedb(); // 更新数据库
		
		return 0;
	}
	else
	{
		// 指定的用户不存在
		trace("修改数据失败，指定的用户不存在");
		return -1;
	}
}

// 删除用户
int CSqliteAccess::DeleteUser(int userid)
{
	// 查询指定的用户
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);

	// 删除用户
	if (user_memy_db.end() != it_user_info)
	{
		list<FACE_MODEL *>::iterator it_face_mod = (*it_user_info)->faces.begin();
		
		// 释放所有用户模板，然后清空链表
		for (; it_face_mod != (*it_user_info)->faces.end(); it_face_mod++)
		{
			delete (*it_face_mod);
		}
		(*it_user_info)->faces.clear();
		delete (*it_user_info);

		it_user_info = user_memy_db.erase(it_user_info);

		updatedb(); // 更新数据库

		return 0;
	}
	else
	{
		// 指定的用户不存在
		trace("删除用户失败，指定的用户不存在");
		return -1;
	}
}

/************************************************************************/
/* 删除用户模板                                                         */
/* @param			userid		[in]		用户ID                    */
/* @param			faceindex	[in]		模板索引                    */
/************************************************************************/
int CSqliteAccess::DeleteFaceModel(int userid, int faceindex)
{
	// 查询指定的用户
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);

	// 删除用户模板
	if (user_memy_db.end() != it_user_info)
	{

		// 判断是否有指定模板
		if (faceindex < (*it_user_info)->faces.size() )
		{
			list<FACE_MODEL *>::iterator it_face_mod = (*it_user_info)->faces.begin();
			for (int i = 0; i < faceindex; i++)
				it_face_mod++;
			
			delete (*it_face_mod);

			it_face_mod = (*it_user_info)->faces.erase(it_face_mod);

		}
		else
		{
			// 指定的模板不存在
			trace("删除模板失败，指定的模板不存在");
			return -1;
		}
		
		updatedb(); // 更新数据库
		return 0;
	}
	else
	{
		// 指定的用户不存在
		trace("删除模板失败，指定的用户不存在");
		return -1;
	}
}

/**
 * 同步数据到数据库
 */
int CSqliteAccess::updatedb( )
{
	// 清空数据库
	::CopyFileA("faceDB.db", DBDIR, FALSE);
	
	// 将内存数据写入数据库
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
	const char *sql_format = "INSERT INTO faces (ID,userid,username,usergender,feat)"
		"VALUES (%d, %d, '%s', '%s', ?);";

	// 构造查询字符串
	char sql_buffer[512] = { 0 };

 	list<USER_INFO *>::iterator it_userinfo = user_memy_db.begin();
	list<FACE_MODEL*>::iterator it_facemode;
	// 开始插入数据
	int did = 0;
	sqlite3_stmt *stat;
	const char *pzTail = NULL;
	for (; it_userinfo != user_memy_db.end(); ++it_userinfo)
	{
		for (it_facemode = (*it_userinfo)->faces.begin(); 
			it_facemode != (*it_userinfo)->faces.end(); ++it_facemode)
		{
			memset(sql_buffer, 0, sizeof sql_buffer);
			sprintf_s(sql_buffer, sizeof sql_buffer, sql_format, did, (*it_userinfo)->OID,
				(*it_userinfo)->user_name, (*it_userinfo)->user_gender);

			sqlite3_prepare(db, sql_buffer, -1, &stat, &pzTail);
			sqlite3_bind_blob(stat, 1, (*it_facemode)->face_feature, 8 *1024, NULL);
			sqlite3_step(stat);

			++did;
		}
	}
	
	sqlite3_finalize(stat);
	sqlite3_close(db);
	return S_OK;
}

