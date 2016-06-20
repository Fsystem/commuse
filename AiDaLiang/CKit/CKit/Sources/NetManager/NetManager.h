#ifndef __NetManager_H
#define __NetManager_H
/** 网络管理
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
	//添加/更新一个服务器(比如核心服务器)
	void AddServer(DWORD dwSvrId,enNetMethod method,LPCSTR szIP,WORD wPort);

	//发送数据（短链接,直接返回数据）-字符串数据
	std::string SendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	std::string TCPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	std::string UDPSendDataByShortLink(DWORD dwSvrId,DWORD dwCMD,std::string strData,INetDelegate* pINetDelegate = NULL);
	void UDPSendDataByShortLinkNoRecv(DWORD dwSvrId,DWORD dwCMD,std::string strData);

	//发送数据（长链接）-暂时没实现

protected:
	
};
#endif //__NetManager_H