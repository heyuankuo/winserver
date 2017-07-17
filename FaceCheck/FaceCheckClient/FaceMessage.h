/**
 * ���Ľ�����
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

	// �ⲿ���ýӿ�
public:
	// ���Ľ����������������������е���Ϣ
	int msgparse(const char *requestmsg, char *responsemsg, int resbuflen);

	// ����������
	int msgproc();

	// ���Ĺ��죬 ��������������Ӧ���ַ���
	int msgcreate(char *responsemsg, int resbuflen);

public:
	/*************************�ͻ��˹��챨��***************************/

	/**
	* ���챨��ͷ
	* @param			dev_id			[in]			�豸���
	* @param			type_msg		[in]			�������� 101 ����XML
	*/
	void FaceCreateHeader(short dev_id, short type_msg);

	/**
	* ����XML
	* @param			xml_version			[in]			xml�汾
	*/
	void FaceCreateClientXML(	const char *msg_version );

	/**
	* �˺���ת��������ͼ�����ݱ���Ϊbase64
	*/
	void TranslateImgCode(unsigned datasize);

	/**
	 * ����ͼ����Ϣ
	 */
	void FaceSetImage(unsigned imgwidth, unsigned imgheight);
	
	// XML �������ʼ��
	void XMLInit();


	/************************���������챨��*************************/

	// ��������
public:

	/************************��������������*************************/
	
	/**
	 * ��ȡhead��Ϣ
	 */
	void ParseMsgHead();

	/**
	* ��ȡxml��Ϣ
	*/
	void ParseClientXml();

	/**
	* ��ȡIMG
	* Ҫ����ȡ��ԭʼ�Ķ�����ͼƬ����
	*/
	void GetMsgImg( const char *filename );

public:
	char message_header[MESSAGE_HEAD];// ����ͷ
	char *str_face_xml;		// XML������
	char *imgdata_base64;	// base64ͼ�����ݻ�����
	char *message_all;		// �������ݱ�������
	unsigned long size_image_base64;			// ͼ������������base64��
	char *imgdata_binary;	// ������ͼ������

public:
	// ����ͷ������
	short id_dev;			// ������豸ID
	short size_xml;			// XML�ߴ�
	short size_all_msg;		// �������ĵĳߴ�

	// xml ������
	unsigned m_flag1;
	unsigned m_userid;
	unsigned m_bh;
	char	 m_username[32];
	char	 m_usergender[8];
	unsigned m_userage;	// �û�����
	unsigned img_width;
	unsigned img_height;
	unsigned img_datasize;
	unsigned resp_result;		// Ӧ���� 0������ѯ�ɹ�������Ϊ������
	char	msg_decs[256];		// ��������

	// ������
	char imgname[128]; // ͼƬ·��

	// ͨ��ģ��
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
