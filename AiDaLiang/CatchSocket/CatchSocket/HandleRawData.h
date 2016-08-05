#ifndef __HandleRawData_H
#define __HandleRawData_H

class HandleRawData : public IHandleRawSocketData
{
public:
	typedef std::vector<char> BinaryArr;
public:
	HandleRawData(DWORD dwPid);
public:
	void WriteHttpData(BinaryArr& szData);
	BinaryArr ReadHttpData();
	//接口实现
protected:
	virtual void HandleData(IPHEADER* pIpHeader);

protected:
	void HandleThread();
	void SendData(IPHEADER* pIpHeader);
	void SendDataByNormalSock(IPHEADER* pIpHeader, const char* pData,int nLen);
private:
	std::list<BinaryArr> mRawHttpDatas;
	CCritiSection mLockHttpsDatas;
	HANDLE mHandleThread;
	DWORD mCatchPid;
	SOCKET mSendSock;
};
#endif //__HandleRawData_H