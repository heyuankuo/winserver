#include "FaceMessage.h"

#include <windows.h>
#include <fstream>
#include <time.h>
#include "tinyxml.h"
#include "dbclass/SqliteAccess.h"

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "tinyxml.lib")

using std::ifstream;
using std::ios_base;
using std::ofstream;

CFaceMessage::CFaceMessage()
{
	str_face_xml	= new char[XML_BUFFERSIZE];			// XML缓冲区
	imgdata_base64	= new char[IMGDATA_BUFFERSIZE];		// base64图像数据
	imgdata_binary	= new char[IMGDATA_BUFFERSIZE];		// 二进制图像数据
}


CFaceMessage::~CFaceMessage()
{
	if (str_face_xml)
	{
		delete[] str_face_xml;
		str_face_xml = NULL;
	}
	
	if (imgdata_base64)
	{
		delete[] imgdata_base64;
		imgdata_base64 = NULL;
	}

	if (imgdata_binary)
	{
		delete[] imgdata_binary;
		imgdata_binary = NULL;
	}
}

/************************************************************************/
/* 外部调用接口，用于客户端和服务器直接调用                                   */
/************************************************************************/

/*
 * 报文解析
 * 此函数为通用函数，用来解析当前定制的报文
 * @param			requestmsg			[in]		对方发过来的请求报文
 * @param			responsemsg			[out]		构造好的用于应答的字符串
 * @param			resbuflen			[in]		应答缓冲区长度
 */
int CFaceMessage::msgparse(const char *requestmsg, char *responsemsg, int resbuflen)
{
	// 检查报文头是否完整

	// 检查关键参数是否完整（ m_flag1， result）
	message_all = (char *)requestmsg;

	ParseMsgHead();		// 提取报文头
	ParseClientXml();	// 提取XML域

	// 根据XML提取图片域
	if (1 == m_flag1 || // 新建用用
		2 == m_flag1 || // 新建模板
		21 == m_flag1)	// 查询人脸
	{
		// 提取图片数据
		const char *file_name_format = "%lf.jpg";
		srand((unsigned)time(NULL));//
		float file_num = rand() % 100;
		memset(imgname, 0, sizeof imgname);
		sprintf_s(imgname, sizeof imgname, file_name_format, file_num);
		GetMsgImg(imgname);
	}
	else if( 100 == m_flag1) // 服务器返回报文
	{
		memset(msg_decs, 0, sizeof msg_decs);
		// 提取图片域数据
		
	}

	return 0;

}

/**
 * 处理报文
 * 此函数处理解析出来的请求报文
 */
int CFaceMessage::msgproc()
{
	CSqliteAccess saq;
	if (1 == m_flag1) // 新建用户请求
	{
		USER_INFO use;
		memcpy_s(use.user_name, sizeof use.user_name, m_username, sizeof use.user_name);
		memcpy_s(use.user_gender, sizeof use.user_gender, m_usergender, sizeof use.user_gender);
		use.user_age = m_userage;

		saq.AddUser(&use, imgname);
		resp_result = 0;
	}
	else if (2 == m_flag1) // 增加模板
	{
		saq.AddFaceMod( m_userid, imgname);
		resp_result = 0;
	}
	else if (11 == m_flag1)
	{
		// 删除用户
		saq.DeleteUser(m_userid);
		resp_result = 0;
	}
	else if (12 == m_flag1)
	{
		// 删除模板
		saq.DeleteFaceModel(m_userid, m_bh);
		resp_result = 0;
	}
	else if (21 == m_flag1) // 人脸对比
	{
		// 人脸查询
		list<USER_INFO *>::iterator it_user;
		saq.GetUserByFace(imgname, &it_user);

		if (it_user != user_memy_db.end())
		{
			// 查询到
			m_userid = (*it_user)->OID;
			memcpy_s(m_username, sizeof m_username, (*it_user)->user_name, sizeof m_username);
			memcpy_s(m_usergender, sizeof m_usergender, (*it_user)->user_gender, sizeof m_usergender);
			m_userage = (*it_user)->user_age;
			resp_result = 0;
		}
		else
		{
			// 未查到
			resp_result = 101;
			char *err_desc = "用户不存在";
			memcpy_s(msg_decs, sizeof msg_decs, err_desc, strlen(err_desc) + 1);
		}
	}
	else if (31 == m_flag1) // 更改数据库数据
	{
		saq.ModifyUseInfo( m_userid, m_username, m_usergender, m_userage);
		resp_result = 0;
	}
	else if (100 == m_flag1) // 服务器返回报文
	{
		
	}

	return 0;
}

/**
 * 报文构造， 函数根据请求类型构造应答字符串
 * @param			responsemsg			[out]			应答缓冲区
 * @param			resbuflen			[in]			应答缓冲区大小
 */
int CFaceMessage::msgcreate(char *responsemsg, int resbuflen)
{
	
#ifdef _FACECHECK
	m_flag1 = 100;
	*imgdata_base64 = NULL; // 清空imgdata_base64缓冲区
#else
	if (1 == m_flag1 ||		// 新建用户请求
		2 == m_flag1 ||		// 增加模板
		21 == m_flag1)		// 人脸查询
	{
		// 转码图片，填写图片属性参数（宽， 高， 数据量）
		if (NULL != strlen(imgname))
		{
			ifstream ifs(imgname, ios_base::binary | ios_base::in);

			// 获取图片文件大小:
			ifs.seekg(0, ios_base::end);
			int length = ifs.tellg();
			ifs.seekg(0, ios_base::beg);

			memset(imgdata_binary, 0, sizeof imgdata_binary);
			ifs.read(imgdata_binary, length);
			ifs.close();
			ifs.clear();

			TranslateImgCode(length); // 转码
			FaceSetImage(100, 100);	// 设置图片宽高
		}
		else
		{
			// 图片文件不存在
			return -1;
		}
	}
	else
	{
		// 清空imgdata_base64缓冲区
		*imgdata_base64 = NULL;
	}
#endif

	// 构造XML 和请求头 
	FaceCreateClientXML("1.0");
	FaceCreateHeader(0, 101);

	// 拼接报文
	memset(responsemsg, 0, resbuflen);
	memcpy_s(responsemsg, resbuflen, message_header, 32);
	memcpy_s(responsemsg + 32, resbuflen - 32, str_face_xml, strlen(str_face_xml));
	if(NULL != strlen(imgdata_base64))
		memcpy_s(responsemsg + 32 + strlen(str_face_xml), resbuflen - 32 - strlen(str_face_xml), imgdata_base64, strlen(imgdata_base64));

	return 0;
}

/**
* 构造报文头
* @param			dev_id			[in]			设备ID
* @param			type_msg		[in]			报文类型
*/
void CFaceMessage::FaceCreateHeader(short dev_id, short type_msg)
{
	// 初始化报文头
	memset(message_header, 0, sizeof message_header);

	// 设置对齐
	message_header[0] = 0x02;
	message_header[1] = 0x07;
	message_header[2] = 0x01;
	message_header[3] = 0x01;

	// 设置包文大小
	UINT *messlen = (UINT *)&message_header[4];
	*messlen = 32 + strlen(str_face_xml) + strlen(imgdata_base64);

	// 设置设备ID
	short *p_id_dev = (short *)&message_header[10];
	*p_id_dev = dev_id;

	// 设置包文类型 101-xml
	short *p_type_msg = (short *)&message_header[12];
	*p_type_msg = type_msg;

	// 设置XML大小
	short *p_size_xml = (short *)&message_header[14];
	*p_size_xml = strlen(str_face_xml);
}

/**
* 构造XML
* @param			xml_version			[in]			xml版本
*/
void CFaceMessage::FaceCreateClientXML( const char *msg_version)
{
	memset(str_face_xml, 0, XML_BUFFERSIZE);
	sprintf_s(str_face_xml, XML_BUFFERSIZE - 1, xml_format,
		msg_version, // xml 版本
		m_flag1,	// 请求类型
		m_userid,	// 用户ID
		m_bh,		// 人脸模板索引
		m_username,	// 用户姓名
		m_usergender, // 用户性别
		m_userage,		// 用户年龄
		img_width,		// 图像宽度
		img_height,		// 图像高度
		img_datasize,	// 图像尺寸
		resp_result);	// 查询结果标志，用于快速判断是否查询成功
}

/**
* 转换图像数据编码为base64
*/
void CFaceMessage::TranslateImgCode( unsigned datasize )
{
	// 初始化缓冲区
	memset(imgdata_base64, 0, IMGDATA_BUFFERSIZE);

	// 转换
	size_image_base64 = IMGDATA_BUFFERSIZE;
	BOOL isok = CryptBinaryToStringA((BYTE *)imgdata_binary,
		datasize,
		CRYPT_STRING_BASE64,
		imgdata_base64,
		&size_image_base64);

	// 异常检测与处理
	if (size_image_base64 > IMGDATA_BUFFERSIZE - 1)
	{
		// 缓冲区太小
	}
}

/**
* 设置图像信息
*/
void CFaceMessage::FaceSetImage(unsigned imgwidth, unsigned imgheight)
{
	img_width = imgwidth;
	img_height = imgheight;
}

// XML 数据域初始化
void CFaceMessage::XMLInit()
{
	m_flag1 = 0;
	m_userid = 0;
	m_bh = 0;
	memset(m_username, 0, sizeof m_username);
	memset(m_usergender, 0, sizeof m_usergender);
	m_userage = 0;
	img_width = 0;
	img_height = 0;
	img_datasize = 0;
	resp_result = 0;		// 应答编号
}

/************************************************************************/
/* 服务器端应答报文 构造                                                       */
/************************************************************************/ 

/*********************报文解析********************************/

/**
* 提取head信息
*/
void CFaceMessage::ParseMsgHead()
{
	// 拷贝报文头
	memcpy_s(message_header, sizeof message_header, message_all, sizeof message_header);

	// 提取数据
	size_all_msg = *((int*)&message_header[4]);
	id_dev = *((short* )&message_header[10]);
	size_xml = *((short*)&message_header[14]);
}

/**
* 提取xml信息
*/
void CFaceMessage::ParseClientXml()
{
	if (NULL == size_xml)
		return;

	// 拷贝XML
	memset(str_face_xml, 0, XML_BUFFERSIZE);
	memcpy_s(str_face_xml, XML_BUFFERSIZE - 1, message_all + MESSAGE_HEAD, size_xml);

	// 解析
	TiXmlDocument doc;
	doc.Parse(str_face_xml);

	const char *ss = NULL;
	TiXmlElement* element = NULL;

	// 请求类型
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Flag1")->ToElement()->GetText();
	if (NULL != ss)
		m_flag1 = atoi(ss);

	// 用户ID
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Userid")->ToElement()->GetText();
	if (NULL != ss)
		m_userid = atoi(ss);

	// 用户模板索引
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("BH")->ToElement()->GetText();
	if (NULL != ss)
		m_bh = atoi(ss);

	// 用户姓名
	memset(m_username, 0, sizeof m_username);
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserName")->ToElement()->GetText();
	if( NULL != ss)
		memcpy_s(m_username, sizeof m_username, ss, strlen(ss));

	// 用户性别
	memset(m_usergender, 0, sizeof m_usergender);
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserGender")->ToElement()->GetText();
	if (NULL != ss)
		memcpy_s(m_usergender, sizeof m_usergender, ss, strlen(ss));

	// 用户年龄
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserAge")->ToElement()->GetText();
	if (NULL != ss)
		m_userage = atoi(ss);

	// 图片宽度
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageWidth")->ToElement()->GetText();
	if (NULL != ss)
		img_width = atoi(ss);

	// 图片高度
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageHeight")->ToElement()->GetText();
	if (NULL != ss)
		img_height = atoi(ss);

	// base64图片文件大小
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageDataSize")->ToElement()->GetText();
	if (NULL != ss)
		size_image_base64 = atoi(ss);

	// 应答编号
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Result")->ToElement()->GetText();
	if (NULL != ss)
		resp_result = atoi(ss);

	doc.Clear(); // 释放
	return;
}

/**
 * 提取报文中的图像保存为图片文件
 * @param		filename   [in]		保存文件名称
 */
void CFaceMessage::GetMsgImg(const char *filename )
{
	// 定位到imgMatrix
	char *img_matrix = (char *)message_all + MESSAGE_HEAD + size_xml;

	// 计算 像素阵列尺寸
	size_image_base64 = size_all_msg - MESSAGE_HEAD - size_xml;

	// 转换编码
	memset(imgdata_binary, 0, sizeof imgdata_binary);
	DWORD len = IMGDATA_BUFFERSIZE;
	CryptStringToBinaryA(img_matrix, size_image_base64, CRYPT_STRING_BASE64, (BYTE *)imgdata_binary, &len, NULL, NULL);

	// 下面是一些测试用的操作
	ofstream ofs(filename, ios_base::binary | ios_base::ate );
	ofs.write( imgdata_binary,len);
	ofs.close();
	ofs.clear();
}