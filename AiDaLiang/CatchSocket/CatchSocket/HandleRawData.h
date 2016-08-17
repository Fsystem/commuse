#ifndef __HandleRawData_H
#define __HandleRawData_H

class HandleRawData : public IHandleRawSocketData
{
public:
	typedef std::vector<char> BinaryArr;
public:
	HandleRawData(DWORD dwPid,bool bCatchProcess = true);
public:
	BOOL StartService();
	void StopService();
	void WriteHttpData(BinaryArr& szData);
	BinaryArr ReadHttpData();

	void WriteHttpData(IPHEADER* pIpData,bool first=false);
	bool ReadHttpData(std::list<TcpSegment::TcpPackageInfo>& packs);
	//接口实现
protected:
	virtual void HandleData(IPHEADER* pIpHeader);

protected:
	void HandleThread();
	void HandleThreadEx();
	void HttpFileThread();
	void SendData(IPHEADER* pIpHeader);
	void SendDataByNormalSock(IPHEADER* pIpHeader, const char* pData,int nLen);
	void SendDataByNormalSockEx(DWORD dwIp,WORD wPort,const char* pData,int nLen,SOCKET sock=INVALID_SOCKET);
private:
	std::list<BinaryArr> mRawHttpDatas;
	TcpSegment mTcpSegments;
	CCritiSection mLockHttpsDatas;
	HANDLE mHandleThread;
	DWORD mCatchPid;
	SOCKET mSendSock;
	bool mCatchProcess;
};
#endif //__HandleRawData_H