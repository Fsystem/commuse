#pragma once

#include <winsock2.h>
//�������緢�ͣ����ش�Э��ͷ
//����ʱ����Ҫע�⣬TCPЭ�飬ͷβ�ֿ�����
//UDPЭ�飬����һ�����
//�ظ�����Ϣ�У�����ȫ�����ܵģ�TCPЭ��������ó�ʱ��5�룬�ͻ��������������������ݣ������Ͽ�������������Ͽ�����

//����Э�鶨��
typedef	struct	_TAG_NET_HEAD
{
	DWORD		tag;  //NETWORK_TAG
	DWORD		ver;	//�汾��
	DWORD		msg_len;	//����
	DWORD		msg_info;	//��Ϣͷ
	DWORD		reserve;	//�����ֶ�

	void	Clear()
	{
		tag = 0;
		ver = 0;
		msg_len = 0;
		msg_info = 0;
		reserve = 0;
	}

	_TAG_NET_HEAD()
	{
		Clear();
	}

	void Code()	//���룬������
	{ 
		tag = htonl(tag);
		msg_len = htonl(msg_len);
		ver = htonl(ver);
		msg_info = htonl(msg_info);
		reserve = htonl(reserve);
	}

	void UnCode()
	{
		tag = ntohl(tag);
		msg_len = ntohl(msg_len);
		ver = ntohl(ver);
		msg_info = ntohl(msg_info);
		reserve = ntohl(reserve);
	}

}TAG_NET_HEAD,*PTAG_NET_HEAD;


//ͳһ�����������ͣ��������£�
//1��100�����ɺ������ʹ�ã��磺�������ط���֮���
//2��100����ÿ100���Σ���һ�����ʹ�ã�
//3) ������ʹ�ã���400����ο�ʼ��ǰ�����Σ��ѷ���


#define		MSG_CENTER_GET_FILEURL_AND_SERVER_Q		11				//��������ҵ�������������ַ
#define		MSG_CENTER_GET_FILEURL_AND_SERVER_A		12

#define		MSG_REPORT_KERNEL_RUN_SUCCESS_Q			21				//���Ĳ���������
#define		MSG_REPORT_KERNEL_RUN_SUCCESS_A			22
