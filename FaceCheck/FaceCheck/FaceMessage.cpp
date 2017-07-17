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
	str_face_xml	= new char[XML_BUFFERSIZE];			// XML������
	imgdata_base64	= new char[IMGDATA_BUFFERSIZE];		// base64ͼ������
	imgdata_binary	= new char[IMGDATA_BUFFERSIZE];		// ������ͼ������
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
/* �ⲿ���ýӿڣ����ڿͻ��˺ͷ�����ֱ�ӵ���                                   */
/************************************************************************/

/*
 * ���Ľ���
 * �˺���Ϊͨ�ú���������������ǰ���Ƶı���
 * @param			requestmsg			[in]		�Է���������������
 * @param			responsemsg			[out]		����õ�����Ӧ����ַ���
 * @param			resbuflen			[in]		Ӧ�𻺳�������
 */
int CFaceMessage::msgparse(const char *requestmsg, char *responsemsg, int resbuflen)
{
	// ��鱨��ͷ�Ƿ�����

	// ���ؼ������Ƿ������� m_flag1�� result��
	message_all = (char *)requestmsg;

	ParseMsgHead();		// ��ȡ����ͷ
	ParseClientXml();	// ��ȡXML��

	// ����XML��ȡͼƬ��
	if (1 == m_flag1 || // �½�����
		2 == m_flag1 || // �½�ģ��
		21 == m_flag1)	// ��ѯ����
	{
		// ��ȡͼƬ����
		const char *file_name_format = "%lf.jpg";
		srand((unsigned)time(NULL));//
		float file_num = rand() % 100;
		memset(imgname, 0, sizeof imgname);
		sprintf_s(imgname, sizeof imgname, file_name_format, file_num);
		GetMsgImg(imgname);
	}
	else if( 100 == m_flag1) // ���������ر���
	{
		memset(msg_decs, 0, sizeof msg_decs);
		// ��ȡͼƬ������
		
	}

	return 0;

}

/**
 * ������
 * �˺����������������������
 */
int CFaceMessage::msgproc()
{
	CSqliteAccess saq;
	if (1 == m_flag1) // �½��û�����
	{
		USER_INFO use;
		memcpy_s(use.user_name, sizeof use.user_name, m_username, sizeof use.user_name);
		memcpy_s(use.user_gender, sizeof use.user_gender, m_usergender, sizeof use.user_gender);
		use.user_age = m_userage;

		saq.AddUser(&use, imgname);
		resp_result = 0;
	}
	else if (2 == m_flag1) // ����ģ��
	{
		saq.AddFaceMod( m_userid, imgname);
		resp_result = 0;
	}
	else if (11 == m_flag1)
	{
		// ɾ���û�
		saq.DeleteUser(m_userid);
		resp_result = 0;
	}
	else if (12 == m_flag1)
	{
		// ɾ��ģ��
		saq.DeleteFaceModel(m_userid, m_bh);
		resp_result = 0;
	}
	else if (21 == m_flag1) // �����Ա�
	{
		// ������ѯ
		list<USER_INFO *>::iterator it_user;
		saq.GetUserByFace(imgname, &it_user);

		if (it_user != user_memy_db.end())
		{
			// ��ѯ��
			m_userid = (*it_user)->OID;
			memcpy_s(m_username, sizeof m_username, (*it_user)->user_name, sizeof m_username);
			memcpy_s(m_usergender, sizeof m_usergender, (*it_user)->user_gender, sizeof m_usergender);
			m_userage = (*it_user)->user_age;
			resp_result = 0;
		}
		else
		{
			// δ�鵽
			resp_result = 101;
			char *err_desc = "�û�������";
			memcpy_s(msg_decs, sizeof msg_decs, err_desc, strlen(err_desc) + 1);
		}
	}
	else if (31 == m_flag1) // �������ݿ�����
	{
		saq.ModifyUseInfo( m_userid, m_username, m_usergender, m_userage);
		resp_result = 0;
	}
	else if (100 == m_flag1) // ���������ر���
	{
		
	}

	return 0;
}

/**
 * ���Ĺ��죬 ���������������͹���Ӧ���ַ���
 * @param			responsemsg			[out]			Ӧ�𻺳���
 * @param			resbuflen			[in]			Ӧ�𻺳�����С
 */
int CFaceMessage::msgcreate(char *responsemsg, int resbuflen)
{
	
#ifdef _FACECHECK
	m_flag1 = 100;
	*imgdata_base64 = NULL; // ���imgdata_base64������
#else
	if (1 == m_flag1 ||		// �½��û�����
		2 == m_flag1 ||		// ����ģ��
		21 == m_flag1)		// ������ѯ
	{
		// ת��ͼƬ����дͼƬ���Բ������� �ߣ� ��������
		if (NULL != strlen(imgname))
		{
			ifstream ifs(imgname, ios_base::binary | ios_base::in);

			// ��ȡͼƬ�ļ���С:
			ifs.seekg(0, ios_base::end);
			int length = ifs.tellg();
			ifs.seekg(0, ios_base::beg);

			memset(imgdata_binary, 0, sizeof imgdata_binary);
			ifs.read(imgdata_binary, length);
			ifs.close();
			ifs.clear();

			TranslateImgCode(length); // ת��
			FaceSetImage(100, 100);	// ����ͼƬ���
		}
		else
		{
			// ͼƬ�ļ�������
			return -1;
		}
	}
	else
	{
		// ���imgdata_base64������
		*imgdata_base64 = NULL;
	}
#endif

	// ����XML ������ͷ 
	FaceCreateClientXML("1.0");
	FaceCreateHeader(0, 101);

	// ƴ�ӱ���
	memset(responsemsg, 0, resbuflen);
	memcpy_s(responsemsg, resbuflen, message_header, 32);
	memcpy_s(responsemsg + 32, resbuflen - 32, str_face_xml, strlen(str_face_xml));
	if(NULL != strlen(imgdata_base64))
		memcpy_s(responsemsg + 32 + strlen(str_face_xml), resbuflen - 32 - strlen(str_face_xml), imgdata_base64, strlen(imgdata_base64));

	return 0;
}

/**
* ���챨��ͷ
* @param			dev_id			[in]			�豸ID
* @param			type_msg		[in]			��������
*/
void CFaceMessage::FaceCreateHeader(short dev_id, short type_msg)
{
	// ��ʼ������ͷ
	memset(message_header, 0, sizeof message_header);

	// ���ö���
	message_header[0] = 0x02;
	message_header[1] = 0x07;
	message_header[2] = 0x01;
	message_header[3] = 0x01;

	// ���ð��Ĵ�С
	UINT *messlen = (UINT *)&message_header[4];
	*messlen = 32 + strlen(str_face_xml) + strlen(imgdata_base64);

	// �����豸ID
	short *p_id_dev = (short *)&message_header[10];
	*p_id_dev = dev_id;

	// ���ð������� 101-xml
	short *p_type_msg = (short *)&message_header[12];
	*p_type_msg = type_msg;

	// ����XML��С
	short *p_size_xml = (short *)&message_header[14];
	*p_size_xml = strlen(str_face_xml);
}

/**
* ����XML
* @param			xml_version			[in]			xml�汾
*/
void CFaceMessage::FaceCreateClientXML( const char *msg_version)
{
	memset(str_face_xml, 0, XML_BUFFERSIZE);
	sprintf_s(str_face_xml, XML_BUFFERSIZE - 1, xml_format,
		msg_version, // xml �汾
		m_flag1,	// ��������
		m_userid,	// �û�ID
		m_bh,		// ����ģ������
		m_username,	// �û�����
		m_usergender, // �û��Ա�
		m_userage,		// �û�����
		img_width,		// ͼ����
		img_height,		// ͼ��߶�
		img_datasize,	// ͼ��ߴ�
		resp_result);	// ��ѯ�����־�����ڿ����ж��Ƿ��ѯ�ɹ�
}

/**
* ת��ͼ�����ݱ���Ϊbase64
*/
void CFaceMessage::TranslateImgCode( unsigned datasize )
{
	// ��ʼ��������
	memset(imgdata_base64, 0, IMGDATA_BUFFERSIZE);

	// ת��
	size_image_base64 = IMGDATA_BUFFERSIZE;
	BOOL isok = CryptBinaryToStringA((BYTE *)imgdata_binary,
		datasize,
		CRYPT_STRING_BASE64,
		imgdata_base64,
		&size_image_base64);

	// �쳣����봦��
	if (size_image_base64 > IMGDATA_BUFFERSIZE - 1)
	{
		// ������̫С
	}
}

/**
* ����ͼ����Ϣ
*/
void CFaceMessage::FaceSetImage(unsigned imgwidth, unsigned imgheight)
{
	img_width = imgwidth;
	img_height = imgheight;
}

// XML �������ʼ��
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
	resp_result = 0;		// Ӧ����
}

/************************************************************************/
/* ��������Ӧ���� ����                                                       */
/************************************************************************/ 

/*********************���Ľ���********************************/

/**
* ��ȡhead��Ϣ
*/
void CFaceMessage::ParseMsgHead()
{
	// ��������ͷ
	memcpy_s(message_header, sizeof message_header, message_all, sizeof message_header);

	// ��ȡ����
	size_all_msg = *((int*)&message_header[4]);
	id_dev = *((short* )&message_header[10]);
	size_xml = *((short*)&message_header[14]);
}

/**
* ��ȡxml��Ϣ
*/
void CFaceMessage::ParseClientXml()
{
	if (NULL == size_xml)
		return;

	// ����XML
	memset(str_face_xml, 0, XML_BUFFERSIZE);
	memcpy_s(str_face_xml, XML_BUFFERSIZE - 1, message_all + MESSAGE_HEAD, size_xml);

	// ����
	TiXmlDocument doc;
	doc.Parse(str_face_xml);

	const char *ss = NULL;
	TiXmlElement* element = NULL;

	// ��������
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Flag1")->ToElement()->GetText();
	if (NULL != ss)
		m_flag1 = atoi(ss);

	// �û�ID
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Userid")->ToElement()->GetText();
	if (NULL != ss)
		m_userid = atoi(ss);

	// �û�ģ������
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("BH")->ToElement()->GetText();
	if (NULL != ss)
		m_bh = atoi(ss);

	// �û�����
	memset(m_username, 0, sizeof m_username);
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserName")->ToElement()->GetText();
	if( NULL != ss)
		memcpy_s(m_username, sizeof m_username, ss, strlen(ss));

	// �û��Ա�
	memset(m_usergender, 0, sizeof m_usergender);
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserGender")->ToElement()->GetText();
	if (NULL != ss)
		memcpy_s(m_usergender, sizeof m_usergender, ss, strlen(ss));

	// �û�����
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("UserAge")->ToElement()->GetText();
	if (NULL != ss)
		m_userage = atoi(ss);

	// ͼƬ���
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageWidth")->ToElement()->GetText();
	if (NULL != ss)
		img_width = atoi(ss);

	// ͼƬ�߶�
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageHeight")->ToElement()->GetText();
	if (NULL != ss)
		img_height = atoi(ss);

	// base64ͼƬ�ļ���С
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("ImageDataSize")->ToElement()->GetText();
	if (NULL != ss)
		size_image_base64 = atoi(ss);

	// Ӧ����
	ss = doc.FirstChild("XMLFaceMessage")->FirstChild("Result")->ToElement()->GetText();
	if (NULL != ss)
		resp_result = atoi(ss);

	doc.Clear(); // �ͷ�
	return;
}

/**
 * ��ȡ�����е�ͼ�񱣴�ΪͼƬ�ļ�
 * @param		filename   [in]		�����ļ�����
 */
void CFaceMessage::GetMsgImg(const char *filename )
{
	// ��λ��imgMatrix
	char *img_matrix = (char *)message_all + MESSAGE_HEAD + size_xml;

	// ���� �������гߴ�
	size_image_base64 = size_all_msg - MESSAGE_HEAD - size_xml;

	// ת������
	memset(imgdata_binary, 0, sizeof imgdata_binary);
	DWORD len = IMGDATA_BUFFERSIZE;
	CryptStringToBinaryA(img_matrix, size_image_base64, CRYPT_STRING_BASE64, (BYTE *)imgdata_binary, &len, NULL, NULL);

	// ������һЩ�����õĲ���
	ofstream ofs(filename, ios_base::binary | ios_base::ate );
	ofs.write( imgdata_binary,len);
	ofs.close();
	ofs.clear();
}