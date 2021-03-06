#pragma once
#include <iostream>
#include <list>
using std::list;


// 用户模板数据结构
typedef struct FACE_MODEL
{
	FACE_MODEL( char *mod):MID(0)
	{
		face_feature = new char[8 * 1024]();
		memcpy_s(face_feature, 8 * 1024, mod, 8 * 1024);
	}

	int MID; // 模板ID
	char *face_feature;
} FACE_MODEL;

// 用户信息
typedef struct
{
	int OID;				// 用户ID
	char user_name[32];		// 用户名
	char user_gender[8];	// 用户性别
	int	user_age;			// 用户年龄
	list<FACE_MODEL *> faces;	// 用户模板集

}USER_INFO;	

extern list<USER_INFO *>user_memy_db; // 内存数据集， 全局唯一


/************************************************************************/
/* 数据库操作类                                                           */
/************************************************************************/
class CSqliteAccess
{
public:
	CSqliteAccess();
	~CSqliteAccess();

public:
	
	// 加载文件数据库到内存数据集
	void LoadData();

	// 根据userid 查询用户数据
	int GetUserByUserid( int userid, list<USER_INFO *>::iterator *it_user);

	// 根据人脸特征查询用户
	int GetUserByFace(char *img_file, list<USER_INFO *>::iterator *it_user );

	// 添加用户
	int AddUser(USER_INFO *user, char *imgname);

	// 添加人脸模板
	int AddFaceMod(int userid, char *imgname);

	// 修改用户数据
	int ModifyUseInfo(int userid, char *username, char *usergender, int userage);

	// 删除用户
	int DeleteUser(int userid );

	// 删除模板
	int DeleteFaceModel(int userid, int faceindex);


	// 同步数据到数据库
	int updatedb();

	static int MAX_USERID;		// 当前最大用户ID 用于添加新用户
};

