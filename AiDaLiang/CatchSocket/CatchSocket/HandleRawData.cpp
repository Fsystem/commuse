#include "stdafx.h"
#include "HandleRawData.h"

extern char TcpFlag[6];



//-------------------------------------------------------------------------------
HandleRawData::HandleRawData(DWORD dwPid)
{
	mHandleThread=NULL;
	mCatchPid = dwPid;

	mSendSock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	DWORD dwErr = GetLastError();
	printf("%u\n",dwErr);
}

void HandleRawData::WriteHttpData(BinaryArr& szData)
{
	LOCK(mLockHttpsDatas);
	mRawHttpDatas.push_back(szData);
}

HandleRawData::BinaryArr HandleRawData::ReadHttpData()
{
	BinaryArr sHttpData;
	LOCK(mLockHttpsDatas);
	if(mRawHttpDatas.size()>0)
	{
		sHttpData = mRawHttpDatas.front();
		mRawHttpDatas.pop_front();
	}
	
	return sHttpData;
}

void HandleRawData::WriteHttpData(IPHEADER* pIpData,bool first)
{
	TCPHEADER* pTcpHeader = (TCPHEADER*)(pIpData+1);
	std::string sData = (char*)(pTcpHeader+1);
	char szKey[256];
	sprintf(szKey,"%u%u%u%u",pIpData->sourceIP,pTcpHeader->sport,pIpData->destIP,pTcpHeader->dport);
	DWORD dwKey = BKDRHash(szKey);
	LOCK(mLockHttpsDatas);
	mTcpSegments.AddPackage(dwKey,pTcpHeader->ack,pIpData->destIP,pTcpHeader->dport,sData,pTcpHeader->flag,first);
	
}

bool HandleRawData::ReadHttpData(std::list<TcpSegment::TcpPackageInfo>& packs)
{
	LOCK(mLockHttpsDatas);
	return mTcpSegments.GetPackage(packs);
}

void HandleRawData::HandleData(IPHEADER* pIpHeader)
{
	//判断是否是TCP包
	if (pIpHeader->proto == IPPROTO_TCP)
	{
		TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);

		char* pSip=(char*)&(pIpHeader->sourceIP),*pDip=(char*)&(pIpHeader->destIP);
		std::ostringstream os;
		os<<"\n**********IP协议头部***********";
		os<<"\n标示："<<ntohs(pIpHeader->ident);
		os<<"\n总长度："<<ntohs(pIpHeader->tatal_len);
		os<<"\n偏移量："<< ntohs(pIpHeader->frag_and_flags);
		os<<"\n生存时间："<<(int)pIpHeader->ttl;
		os<<"\n服务类型："<<(int)pIpHeader->tos;
		os<<"\n协议类型："<<(int)pIpHeader->proto;
		os<<"\n检验和："<< ntohs(pIpHeader->checksum);
		os<<"\n源IP："<< (int)(byte)pSip[0]<<"."<<(int)(byte)pSip[1]<<"."<<(int)(byte)pSip[2]<<"."<<(int)(byte)pSip[3];
		os<<"\n目的IP："<< (int)(byte)pDip[0]<<"."<<(int)(byte)pDip[1]<<"."<<(int)(byte)pDip[2]<<"."<<(int)(byte)pDip[3];

		//std::cout<<os.str();
		os.str("");

		os<<"\n#################数据包长度%d"<<pIpHeader->tatal_len-sizeof(IPHEADER)-sizeof(TCPHEADER);
		os<<"\n**********TCP协议头部***********";
		os<<"\n源端口："<<ntohs(pTcpheader->sport);
		os<<"\n目的端口："<< ntohs(pTcpheader->dport);
		os<<"\n序列号："<< ntohs(pTcpheader->seq);
		os<<"\n应答号："<< ntohs(pTcpheader->ack);
		os<<"\n检验和："<< ntohs(pTcpheader->sum);
		os<<"\n标志位：";
		//打印标志位
		unsigned char FlagMask = 1;
		for (int k = 0; k < 6; k++)
		{
			if ((pTcpheader->flag)&FlagMask)
				os<<(char)TcpFlag[k];
			else
				os<<" ";
			FlagMask = FlagMask << 1;
		}
		
		DWORD dwSockPid = GetPidBySocketLink(pIpHeader->sourceIP,pTcpheader->sport,pIpHeader->destIP,pTcpheader->dport);
		os<<"\nPID："<< dwSockPid<<"\n";
		
		//sLog+= os.str();
		//std::cout<<os.str();

		//WriteDataToFile("IPHead.txt",sLog.c_str());
		if( dwSockPid == 0) return;
		if(dwSockPid == GetCurrentProcessId()) return;
		if(mCatchPid != 0 && dwSockPid != mCatchPid) return;
		
		//判断是不是HTTP协议请求
		char* pData = (char*)(pTcpheader+1);
		//std::ofstream of("./ack.log",std::ios::app);
		if (strstr(pData,"HTTP") && (strnicmp(pData,"POST",4)==0 || strnicmp(pData,"GET",3)==0))
		{
			//of<<"HTTP:"<<pTcpheader->ack<<pData <<std::endl;
			printf("\nHTTP数据:%s\n",pData);
			//WriteDataToFile("./HttpData.txt",pData);
			std::vector<char> sHttpData;
			sHttpData.assign((char*)pIpHeader,(char*)pIpHeader+pIpHeader->tatal_len);

			WriteHttpData(pIpHeader,true);

			if (mHandleThread == NULL)
			{
				mHandleThread = (HANDLE)JKThread<HandleRawData>::Start(&HandleRawData::HandleThreadEx,this);
			}

		}
		//else if (strstr(pData,"HTTP"))
		//{
		//	std::ofstream of("./httpres.log",std::ios::app);
		//	if (of.is_open())
		//	{
		//		of<<pData<<std::endl;
		//	}
		//}
		else
		{
			WriteHttpData(pIpHeader,false);
			//of<<"ack:"<<pTcpheader->ack<<pData<<std::endl;
		}
	}
}

void HandleRawData::HandleThreadEx()
{
	std::ofstream of0("./httpData.log",std::ios::out);
	std::ofstream ofbegin0("./Send.log",std::ios::out);
	of0.close();
	ofbegin0.close();

	while(1)
	{
		std::list<TcpSegment::TcpPackageInfo> infos;
		if (ReadHttpData(infos))
		{
			std::ofstream of("./httpData.log",std::ios::app);

			SOCKADDR_IN addr_in;
			memset(&addr_in,0,sizeof(addr_in));
			addr_in.sin_family=AF_INET;
			addr_in.sin_port=infos.front().mPort;
			addr_in.sin_addr.S_un.S_addr=infos.front().mIp;

			//
			SOCKET sendSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			//int flag=1,len=sizeof(int);
			//setsockopt(sendSock, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, len);
			int nError = connect(sendSock,(struct sockaddr*)&addr_in,sizeof(addr_in));

			for(auto it = infos.begin();it!=infos.end();it++)
			{
			std::string sData = it->sData;
			std::cout<<sData<<std::endl;
			if (of.is_open())
			{
			of<<sData<<std::endl;
			}

			SendDataByNormalSockEx(it->mIp,it->mPort,sData.c_str(),sData.size(),sendSock);
			}

			shutdown(sendSock,SD_BOTH);
			closesocket(sendSock);
			
		}
		else
		{
			Sleep(100);
		}
		Sleep(10);
	}
}


void HandleRawData::HandleThread()
{
	std::ofstream of0("./httpData.log",std::ios::out);
	std::ofstream ofbegin0("./Send.log",std::ios::out);
	of0.close();
	ofbegin0.close();

	while(1)
	{
		BinaryArr arr = ReadHttpData();
		if (arr.size() > 0)
		{
			IPHEADER* pIpHeader = (IPHEADER*)&arr[0];
			TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);
			std::string sData = (char*)(pTcpheader+1);

			std::cout<<sData<<std::endl;
			std::ofstream of("./httpData.log",std::ios::app);
			if (of.is_open())
			{
				of<<sData<<std::endl;
			}

			size_t idx = sData.find("Content-Length:");
			
			int nCLen = 0;
			if( idx != std::string::npos)
			{
				std::string sConLen = sData.substr(idx+strlen("Content-Length:"),std::string::npos);
				size_t idxEnd = sConLen.find("\r\n");
				if(idxEnd != std::string::npos) sConLen = sConLen.substr(0,idxEnd);
				nCLen = std::stoi(sConLen);
			}
			//SendDataByNormalSock(pIpHeader,(char*)(pTcpheader+1),pIpHeader->tatal_len-sizeof(IPHEADER)-sizeof(TCPHEADER));
			SendDataByNormalSock(pIpHeader,sData.c_str(),sData.size()+nCLen);
		}
		else
		{
			Sleep(100);
		}
		Sleep(10);
	}
}

void HandleRawData::SendDataByNormalSock(IPHEADER* pIpHeader,const char* pData,int nLen)
{
	TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);
	SendDataByNormalSockEx(pIpHeader->destIP,pTcpheader->dport,pData,nLen);
}

void HandleRawData::SendDataByNormalSockEx(DWORD dwIp,WORD wPort,const char* pData,int nLen,SOCKET sock)
{
	std::ostringstream os;
	std::ofstream ofSend("./Send.log",std::ios::app);

	std::string sData = pData;

	SOCKET sendSock = sock;
	bool bClean = false;
	if (sendSock == INVALID_SOCKET)
	{
		bClean = true;

		SOCKADDR_IN addr_in;
		memset(&addr_in,0,sizeof(addr_in));
		addr_in.sin_family=AF_INET;
		addr_in.sin_port=wPort;
		addr_in.sin_addr.S_un.S_addr=dwIp;

		sendSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if(SOCKET_ERROR == connect(sendSock,(struct sockaddr*)&addr_in,sizeof(addr_in)) )
		{
			os<<"连接失败:["<<GetLastError()<<"]"<<pData<<std::endl;
			goto CLEAR;
		}
	}

	//linger lin;
	//lin.l_linger=0;
	//lin.l_onoff=1;
	//if( SOCKET_ERROR == setsockopt(sendSock,SOL_SOCKET,SO_LINGER,(char*)&lin,sizeof(linger)) )
	//{
	//	os<<"设置SO_LINGER失败:["<<GetLastError()<<"]"<<pData<<std::endl;
	//	goto CLEAR;
	//}
	
	
	if(SOCKET_ERROR == send(sendSock,pData,nLen,0) )
	{
		os<<"发送失败:["<<GetLastError()<<"]"<<pData<<std::endl;
		goto CLEAR;
	}
	else
	{
		os<<"发送成功:"<<pData<<std::endl;
		goto CLEAR;
	}
	//closesocket(sendSock);

CLEAR:
	if (ofSend.is_open())
	{
		ofSend<<os.str();
	}
	if (sendSock == INVALID_SOCKET)
	if(bClean)shutdown(sendSock,SD_BOTH);
}


void HandleRawData::SendData(IPHEADER* pIpHeader)
{
	TCPHEADER* pTcpheader = (TCPHEADER*)(pIpHeader+1);
	char* pData = (char*)(pTcpheader+1);

	SOCKADDR_IN addr_in;
	memset(&addr_in,0,sizeof(addr_in));
	addr_in.sin_family=AF_INET;
	addr_in.sin_port=0;
	addr_in.sin_addr.S_un.S_addr=pIpHeader->destIP;

	SOCKET sendSock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	int nErr = 0;
	int one = 1;

	const int *val = &one;
	nErr = setsockopt(sendSock, IPPROTO_IP, IP_HDRINCL, (char*)val, sizeof(one));

	nErr = ::bind(sendSock,(SOCKADDR*)&addr_in,sizeof(addr_in));
	if (nErr != SOCKET_ERROR)
	{
		int nLen = sizeof(addr_in);
	}

	pTcpheader->sport=addr_in.sin_port;

	//计算校验和
	int nDataLen = strlen(pData);
	char *szSendBuf=new char[sizeof(IPHEADER) + sizeof(TCPHEADER) + nDataLen];
	IPHEADER ipHeader;
	TCPHEADER tcpHeader;
	PSDHEADER psdHeader;

	//填充IP首部 
	ipHeader.h_verlen=( 4<<4 | sizeof(ipHeader)/sizeof(unsigned long)); 
	//ipHeader.tos=0; 
	ipHeader.tatal_len = htons(sizeof(ipHeader) + sizeof(tcpHeader)); 
	ipHeader.ident=1; 
	ipHeader.frag_and_flags=0; 
	ipHeader.ttl = 128; 
	ipHeader.proto = IPPROTO_TCP; 
	ipHeader.checksum = 0; 
	ipHeader.sourceIP = pIpHeader->sourceIP; 
	ipHeader.destIP = pIpHeader->destIP;

	//填充TCP首部 
	tcpHeader.dport = pTcpheader->dport;  //目标端口
	tcpHeader.sport=pTcpheader->sport; //源端口号 
	tcpHeader.seq = htonl(0x12345678); 
	tcpHeader.ack=0; 
	tcpHeader.lenres = (sizeof(tcpHeader) / 4 <<4| 0); 
	tcpHeader.flag = 2; //修改这里来实现不同的标志位探测，2是SYN，1是FIN，16是ACK探测 等等 
	tcpHeader.win= htons(512); 
	tcpHeader.urp=0; 
	tcpHeader.sum=0; 

	//填充TCP伪首部
	psdHeader.saddr=pIpHeader->sourceIP;
	psdHeader.daddr=pIpHeader->destIP;
	psdHeader.mbz=0;
	psdHeader.ptcl=IPPROTO_TCP;
	psdHeader.tcpl=htons(sizeof(TCPHEADER));

	memcpy(szSendBuf, &psdHeader, sizeof(psdHeader));
	memcpy(szSendBuf+sizeof(psdHeader), &tcpHeader, sizeof(TCPHEADER));
	tcpHeader.sum=ChechSum((USHORT *)szSendBuf,sizeof(psdHeader)+sizeof(TCPHEADER));

	memcpy(szSendBuf, &ipHeader, sizeof(IPHEADER));
	memcpy(szSendBuf+sizeof(IPHEADER), &tcpHeader, sizeof(tcpHeader));
	memset(szSendBuf+sizeof(IPHEADER)+sizeof(TCPHEADER), 0, 4);
	ipHeader.checksum=ChechSum((USHORT *)szSendBuf, sizeof(IPHEADER)+sizeof(TCPHEADER));

	memcpy(szSendBuf, &ipHeader, sizeof(ipHeader)); 
	memcpy(szSendBuf + sizeof(ipHeader)+sizeof(tcpHeader),pData,nDataLen);

	addr_in.sin_family=AF_INET;
	addr_in.sin_port=pTcpheader->dport;
	addr_in.sin_addr.S_un.S_addr=pIpHeader->destIP;

	
	WORD wDport = ntohs(addr_in.sin_port);

	std::ofstream ofSend("./Send.log",std::ios::app);
	if( SOCKET_ERROR == sendto(sendSock,(char*)szSendBuf,sizeof(ipHeader)+sizeof(tcpHeader) + nDataLen,0,(struct sockaddr*)&addr_in,sizeof(addr_in)) )
	{
		if (ofSend.is_open())
		{
			ofSend<<"发送失败:["<<GetLastError()<<"]"<<pData<<std::endl;
		}
	}
	else
	{
		if (ofSend.is_open())
		{
			ofSend<<"发送成功:"<<pData<<std::endl;
		}
	}

	shutdown(sendSock,SD_BOTH);

	delete szSendBuf;
}