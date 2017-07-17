#include "SqliteAccess.h"
#include "sqlite3.h"
#include "../SeetaFace.h"
#include <Windows.h>
#pragma comment(lib, "dbclass/sqlite3.lib")

const char *DBDIR = "E:\\faceDB.db";

list<USER_INFO *>user_memy_db; // �ڴ����ݿ⣬ ȫ��Ψһ


// ��¼��־
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
* �������ݿ�����
* �˺����ڳ���ʼʱ�����������ݿ����ݵ��ڴ��С�
* �˺����������OID
* @param			maxuserid			[in, out]		��������û�ID
*/
void CSqliteAccess::LoadData(  )
{
	// ����ڴ����ݼ�
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

	// �����ݿ�
	sqlite3 *db = NULL;
	int result = sqlite3_open(DBDIR, &db);

	// ��ȡ��������
	sqlite3_stmt *stat = NULL;
	const char *pzTail = NULL;
	sqlite3_prepare(db, "select * from faces;", -1, &stat, &pzTail);
	// sqlite3_column_count(stat);	// ��¼����

	MAX_USERID = 0;
	result = sqlite3_step(stat);

	// ������������
	while (result == SQLITE_ROW)
	{
		// ��ѯ�ڴ����ݼ����Ƿ����е�ǰ�û�
		int current_userid = sqlite3_column_int(stat, 1); // ��ȡ��ǰuserid
		MAX_USERID = (MAX_USERID > current_userid) ? MAX_USERID : current_userid;

		// �����û�����
		GetUserByUserid(current_userid, &it_user_info);	
		if (user_memy_db.end() != it_user_info)
		{
			(*it_user_info)->faces.push_back(new FACE_MODEL((char *)sqlite3_column_blob(stat, 4)));

		} // �Ѿ��е�ǰ�û�����
		else
		{
			// �������û�
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

		}// ��û�е�ǰ�û�����

		result = sqlite3_step(stat);
	}

	sqlite3_finalize(stat);
	sqlite3_close(db);
	return ;
}

/**
 * ʹ��userid ��ѯ�û�����
 * @pararm		userid		[in]		
 * @pararm		it_user		[out]		��Ҫ���صĵ�����
 */
int CSqliteAccess::GetUserByUserid(int userid, list<USER_INFO *>::iterator *it_user)
{
	if (NULL == it_user)
	{
		trace("ʹ��userid��ѯ�û����ݳ���ԭ��������ĵ�����ָ��Ϊ��");
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
* ʹ��facemod ��ѯ�û�����
* @pararm		img_file	[in]		��Ƭ·��
* @pararm		it_user		[out]		��Ҫ���صĵ�����
*/
int CSqliteAccess::GetUserByFace(char *img_file, list<USER_INFO *>::iterator *it_user)
{
	if (NULL == img_file)
	{
		trace("�����·��Ϊ��");
		return -1;
	}

	// ��ȡ��������
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(img_file, feat);

	float result = 0.0f; // �����жϽ�����棻
	list<USER_INFO *>::iterator it_index = user_memy_db.begin(); // ���ڱ������ݿ�ĵ�����

	// ��ʼ�ȶ�
	// (*it_user) = 
	for (; it_index != user_memy_db.end(); it_index++)
	{
		for( list<FACE_MODEL *>::iterator it_face_mod = (*it_index)->faces.begin();
			it_face_mod != (*it_index)->faces.end(); it_face_mod++)
		{
			float match_tmp = sf.FeatureCompare( (float *)(*it_face_mod)->face_feature, feat); // ����ƥ���

			if (0.8f < match_tmp)
			{
				result = match_tmp;
				*it_user = it_index;
				break;			// ����⵽0.8 ����ƥ�䣬ֱ������
			}

			if (0.60f > match_tmp)
				continue;		// ƥ����0.6 ���£�ֱ������

			// ��0.8-0.6 ֮�䣬ѡ��ƥ��ȸߵ�
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
/* ������û�																*/
/************************************************************************/
int CSqliteAccess::AddUser(USER_INFO *user, char *imgname)
{
	if (NULL == user)
	{
		trace("������û�ʧ�ܣ� ԭ�����������Ϊ��");
		return -1;
	}

	// ��ȡ��������
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(imgname, feat);

	InterlockedIncrement((LONG *)&MAX_USERID); // ����û�ID����

	// �����û���Ϣ
	USER_INFO *puser = new USER_INFO();
	puser->OID = MAX_USERID;
	memcpy_s(puser->user_name, sizeof puser->user_name, user->user_name, sizeof puser->user_name);
	memcpy_s(puser->user_gender, sizeof puser->user_gender, user->user_gender, sizeof puser->user_gender);
	puser->user_age = user->user_age;

	// ���ģ��
	puser->faces.push_back(new FACE_MODEL((char *)feat));

	user_memy_db.push_back(puser);

	updatedb(); // �������ݿ�

	return 0;

}

/************************************************************************/
/* �������û��������ģ��                                                           */
/************************************************************************/
int CSqliteAccess::AddFaceMod(int userid, char *imgname)
{
	// ��ȡ��������
	CSeetaFace sf;
	float *feat = NULL;
	feat = sf.NewFeatureBuffer();
	int vt = sf.GetFeatureDims();
	sf.GetFeature(imgname, feat);

	// ��ѯָ�����û�
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);	

	if (user_memy_db.end() != it_user_info)
	{
		(*it_user_info)->faces.push_back(new FACE_MODEL((char *)feat));
		return 0;
	}
	else
	{
		// ָ�����û�������
		trace("���ģ��ʧ�ܣ�ָ�����û�������");
		return -1;
	}

	updatedb(); // �������ݿ�
}

// �޸��û�����
int CSqliteAccess::ModifyUseInfo(int userid, char *username, char *usergender, int userage)
{
	// ��ѯָ�����û�
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

		updatedb(); // �������ݿ�
		
		return 0;
	}
	else
	{
		// ָ�����û�������
		trace("�޸�����ʧ�ܣ�ָ�����û�������");
		return -1;
	}
}

// ɾ���û�
int CSqliteAccess::DeleteUser(int userid)
{
	// ��ѯָ�����û�
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);

	// ɾ���û�
	if (user_memy_db.end() != it_user_info)
	{
		list<FACE_MODEL *>::iterator it_face_mod = (*it_user_info)->faces.begin();
		
		// �ͷ������û�ģ�壬Ȼ���������
		for (; it_face_mod != (*it_user_info)->faces.end(); it_face_mod++)
		{
			delete (*it_face_mod);
		}
		(*it_user_info)->faces.clear();
		delete (*it_user_info);

		it_user_info = user_memy_db.erase(it_user_info);

		updatedb(); // �������ݿ�

		return 0;
	}
	else
	{
		// ָ�����û�������
		trace("ɾ���û�ʧ�ܣ�ָ�����û�������");
		return -1;
	}
}

/************************************************************************/
/* ɾ���û�ģ��                                                         */
/* @param			userid		[in]		�û�ID                    */
/* @param			faceindex	[in]		ģ������                    */
/************************************************************************/
int CSqliteAccess::DeleteFaceModel(int userid, int faceindex)
{
	// ��ѯָ�����û�
	list<USER_INFO *>::iterator it_user_info;
	GetUserByUserid(userid, &it_user_info);

	// ɾ���û�ģ��
	if (user_memy_db.end() != it_user_info)
	{

		// �ж��Ƿ���ָ��ģ��
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
			// ָ����ģ�岻����
			trace("ɾ��ģ��ʧ�ܣ�ָ����ģ�岻����");
			return -1;
		}
		
		updatedb(); // �������ݿ�
		return 0;
	}
	else
	{
		// ָ�����û�������
		trace("ɾ��ģ��ʧ�ܣ�ָ�����û�������");
		return -1;
	}
}

/**
 * ͬ�����ݵ����ݿ�
 */
int CSqliteAccess::updatedb( )
{
	// ������ݿ�
	::CopyFileA("faceDB.db", DBDIR, FALSE);
	
	// ���ڴ�����д�����ݿ�
	sqlite3 *db;
	int rc;

	// �����ݿ�
	rc = sqlite3_open(DBDIR, &db);
	if (rc)
	{
		// ���ݿ��ʧ��
		return E_FAIL;
	}

	// ��ѯ�ַ���ģ��
	const char *sql_format = "INSERT INTO faces (ID,userid,username,usergender,feat)"
		"VALUES (%d, %d, '%s', '%s', ?);";

	// �����ѯ�ַ���
	char sql_buffer[512] = { 0 };

 	list<USER_INFO *>::iterator it_userinfo = user_memy_db.begin();
	list<FACE_MODEL*>::iterator it_facemode;
	// ��ʼ��������
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

