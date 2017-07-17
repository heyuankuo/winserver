/**
 * 报文解析类
 */
#pragma once
#include <iostream>
#define		MESSAGE_HEAD			32
#define		IMGDATA_BUFFERSIZE		3 * 1024 * 1024
#define		XML_BUFFERSIZE			1024 * 2

class CFaceMessage
{
public:
	CFaceMessage();
	~CFaceMessage();

	// 外部调用接口
public:
	// 报文解析，函数解析报文中所有的信息
	int msgparse(const char *requestmsg, char *responsemsg, int resbuflen);

	// 报文请求处理
	int msgproc();

	// 报文构造， 函数根据请求构造应答字符串
	int msgcreate(char *responsemsg, int resbuflen);

public:
	/*************************客户端构造报文***************************/

	/**
	* 构造报文头
	* @param			dev_id			[in]			设备编号
	* @param			type_msg		[in]			报文类型 101 代表XML
	*/
	void FaceCreateHeader(short dev_id, short type_msg);

	/**
	* 构造XML
	* @param			xml_version			[in]			xml版本
	*/
	void FaceCreateClientXML(	const char *msg_version );

	/**
	* 此函数转换二进制图像数据编码为base64
	*/
	void TranslateImgCode(unsigned datasize);

	/**
	 * 设置图像信息
	 */
	void FaceSetImage(unsigned imgwidth, unsigned imgheight);
	
	// XML 数据域初始化
	void XMLInit();


	/************************服务器构造报文*************************/

	// 解析报文
public:

	/************************服务器解析报文*************************/
	
	/**
	 * 提取head信息
	 */
	void ParseMsgHead();

	/**
	* 提取xml信息
	*/
	void ParseClientXml();

	/**
	* 提取IMG
	* 要求提取到原始的二进制图片数据
	*/
	void GetMsgImg( const char *filename );

public:
	char message_header[MESSAGE_HEAD];// 报文头
	char *str_face_xml;		// XML缓冲区
	char *imgdata_base64;	// base64图像数据缓冲区
	char *message_all;		// 完整数据报缓冲区
	unsigned long size_image_base64;			// 图像数据量（按base64）
	char *imgdata_binary;	// 二进制图像数据

public:
	// 报文头数据项
	short id_dev;			// 请求的设备ID
	short size_xml;			// XML尺寸
	short size_all_msg;		// 整个报文的尺寸

	// xml 数据项
	unsigned m_flag1;
	unsigned m_userid;
	unsigned m_bh;
	char	 m_username[32];
	char	 m_usergender[8];
	unsigned m_userage;	// 用户年龄
	unsigned img_width;
	unsigned img_height;
	unsigned img_datasize;
	unsigned resp_result;		// 应答编号 0代表查询成功，其他为错误码
	char	msg_decs[256];		// 错误描述

	// 其他项
	char imgname[128]; // 图片路径
	// 通用xml 模板
	const char *xml_format = "<? xml version = \"1.0\" ?>\r\n"
							"<XMLFaceMessage>\r\n"
							"<FaceVersion>%s</FaceVersion>\r\n"
							"<Flag1>%d</Flag1>\r\n"
							"<Userid>%d</Userid>\r\n"
							"<BH>%d</BH>\r\n"
							"<UserName>%s</UserName>\r\n"
							"<UserGender>%s</UserGender>\r\n"
							"<UserAge>%d</UserAge>\r\n"
							"<ImageWidth>%d</ImageWidth>\r\n"
							"<ImageHeight>%d</ImageHeight>\r\n"
							"<ImageDataSize>%d</ImageDataSize>\r\n"
							"<Result>%d</Result>\r\n"
							"</XMLFaceMessage>";	
};

