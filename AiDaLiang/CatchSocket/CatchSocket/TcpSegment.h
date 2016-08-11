#ifndef __TcpSegment_H
#define __TcpSegment_H
class TcpSegment
{
public:
	struct TcpPackageInfo
	{
		DWORD mIp;
		WORD mPort;
		std::string sData;
		bool bFinish;
		DWORD dwTime;

		TcpPackageInfo()
		{
			dwTime = 0;
			mIp = 0;
			mPort = 0;
			sData = "";
			bFinish = false;
		}
	};
public:
	void AddPackage(DWORD dwKey,DWORD dwAck,DWORD sIp,WORD wPort,std::string sData,byte cbFlag,bool fisrt = false);
	bool GetPackage(std::list<TcpSegment::TcpPackageInfo>& packs);
protected:
private:
	std::map<DWORD,std::map<DWORD,TcpPackageInfo>> mTcpSegments;
	
};
#endif //__TcpSegment_H