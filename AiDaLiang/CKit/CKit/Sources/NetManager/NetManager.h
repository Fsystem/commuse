#ifndef __NetManager_H
#define __NetManager_H
/** �������
*   @FileName  : NetManager.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-6-7
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

class NetManager : public SingletonBase<NetManager>
{
public:
	NetManager();
	virtual ~NetManager();

public:
	//���/����һ��������(������ķ�����)
	void AddServer(DWORD dwSvrId,enNetMethod method,LPCSTR szIP,WORD wPort);

	//�������ݣ�������,ֱ�ӷ������ݣ�-�ַ�������
	std::string SendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	std::string TCPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	std::string UDPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	void UDPSendDataByShortLinkNoRecv(DWORD dwSvrId,DWORD dwCMD,std::string strData);

	//�������ݣ������ӣ�-��ʱûʵ��

protected:
	
};
#endif //__NetManager_H