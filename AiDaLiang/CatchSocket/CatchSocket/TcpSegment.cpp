#include "stdafx.h"
#include "TcpSegment.h"

void TcpSegment::AddPackage(DWORD dwKey,DWORD dwAck,DWORD sIp,WORD wPort,std::string sData,byte cbFlag,bool fisrt)
{
	std::ostringstream os;
	std::ofstream of("packet.log",std::ios::app);

	if(fisrt)
	{
		//if (int(cbFlag&0x8) != 0)
		{
			TcpPackageInfo info = mTcpSegments[dwKey][dwAck];
			info.mIp = sIp;
			info.mPort = wPort;
			info.sData += sData;
			info.dwTime = GetTickCount();

			mTcpSegments[dwKey][dwAck] = info;

			os<<"[1]"<<dwKey<<":"<<dwAck<<":"<<":"<<(int)(cbFlag&0x8)<<sData<<std::endl;
			if (of.is_open())
			{
				of<<os.str();
			}
		}
	}
	else
	{
		auto it  = mTcpSegments.find(dwKey);

		if (it != mTcpSegments.end())
		{
			if (it->second.find(dwAck)!=it->second.end())
			{
				it->second[dwAck].sData += sData;
			}
			
			os<<"[2]"<<dwKey<<":"<<dwAck<<":"<<":"<<(int)(cbFlag&0x8)<<sData<<std::endl;
			if (of.is_open())
			{
				of<<os.str();
			}

			if (it->second.begin()->second.bFinish == false && int(cbFlag&0x8) == 0)
			{
				os<<"[3]"<<dwKey<<":"<<dwAck<<":"<<":"<<(int)(cbFlag&0x8)<<sData<<std::endl;
				if (of.is_open())
				{
					of<<os.str();
				}

				for (auto it1 = it->second.begin();it1!=it->second.end();it1++)
				{
					it1->second.bFinish = true;
				}
			}
		}

	}
	
}

bool  TcpSegment::GetPackage(std::list<TcpSegment::TcpPackageInfo>& packs)
{
	for (auto it = mTcpSegments.begin();it!=mTcpSegments.end();it++)
	{
		if (it->second.begin()->second.bFinish || (GetTickCount() - it->second.begin()->second.dwTime)>=6000 )
		{
			for (auto it1 = it->second.begin();it1!=it->second.end();it1++)
			{
				packs.push_back(it1->second);
			}
			mTcpSegments.erase(it);
			printf(">>>>>>>>>[%d]\n",mTcpSegments.size());
			return true;
		}
	}

	return false;
}