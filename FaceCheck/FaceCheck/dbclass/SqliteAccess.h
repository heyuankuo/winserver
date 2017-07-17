#pragma once
#include <iostream>
#include <list>
using std::list;


// �û�ģ�����ݽṹ
typedef struct FACE_MODEL
{
	FACE_MODEL( char *mod):MID(0)
	{
		face_feature = new char[8 * 1024]();
		memcpy_s(face_feature, 8 * 1024, mod, 8 * 1024);
	}

	int MID; // ģ��ID
	char *face_feature;
} FACE_MODEL;

// �û���Ϣ
typedef struct
{
	int OID;				// �û�ID
	char user_name[32];		// �û���
	char user_gender[8];	// �û��Ա�
	int	user_age;			// �û�����
	list<FACE_MODEL *> faces;	// �û�ģ�弯

}USER_INFO;	

extern list<USER_INFO *>user_memy_db; // �ڴ����ݼ��� ȫ��Ψһ


/************************************************************************/
/* ���ݿ������                                                           */
/************************************************************************/
class CSqliteAccess
{
public:
	CSqliteAccess();
	~CSqliteAccess();

public:
	
	// �����ļ����ݿ⵽�ڴ����ݼ�
	void LoadData();

	// ����userid ��ѯ�û�����
	int GetUserByUserid( int userid, list<USER_INFO *>::iterator *it_user);

	// ��������������ѯ�û�
	int GetUserByFace(char *img_file, list<USER_INFO *>::iterator *it_user );

	// ����û�
	int AddUser(USER_INFO *user, char *imgname);

	// �������ģ��
	int AddFaceMod(int userid, char *imgname);

	// �޸��û�����
	int ModifyUseInfo(int userid, char *username, char *usergender, int userage);

	// ɾ���û�
	int DeleteUser(int userid );

	// ɾ��ģ��
	int DeleteFaceModel(int userid, int faceindex);


	// ͬ�����ݵ����ݿ�
	int updatedb();

	static int MAX_USERID;		// ��ǰ����û�ID ����������û�
};

