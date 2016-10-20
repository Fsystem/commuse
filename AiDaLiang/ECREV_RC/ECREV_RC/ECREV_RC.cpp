// ECREV_RC.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/regex.h>
#include<stdio.h>
#include <algorithm>
#include <io.h>

#pragma comment(linker,"/subsystem:windows")

std::locale loc("chs");

int APIENTRY _tWinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in LPTSTR lpCmdLine, __in int nShowCmd )
{
	std::string s="asdsASDADasdfdf11";
	std::transform(s.begin(),s.end(),s.begin(),::toupper);

	if (__argc>2)
	{
		//std::wstring wDesPathFile = A2WString(__argv[2]);

		//OutputDebugStringA(__argv[1]);
		//OutputDebugStringA(__argv[2]);
		std::string sRcFile = T2AString(__targv[1]);
		std::string sRcFileTpl = T2AString(__targv[2]);
		if (_access(sRcFile.c_str(),0) == -1)
		{
			OutputDebugStringA("没有资源文件\n");
			return 0;
		}
		CopyFileA(sRcFile.c_str(),sRcFileTpl.c_str(),FALSE);

		FILE* fp = fopen(sRcFileTpl.c_str(),"rb");
		fseek(fp,0,SEEK_END);
		int nLen = ftell(fp);
		byte* szData = new byte[nLen+1];
		rewind(fp);
		fread(szData,1,nLen,fp);
		fclose(fp);
		szData[nLen] = 0;

		std::wstring sContent;
		//UNICODE文件
		if (!(szData[0]==0xff&&szData[1]==0xfe || szData[0]==0xfe&&szData[1]==0xff))
		{
			sContent = A2WString((LPCSTR)szData);
		}
		else sContent.assign((LPWSTR)(szData+2),nLen/2-1);
		//else sContent.assign((LPWSTR)(szData),nLen/2);
		delete[] szData;

		//std::string sContent;// = sbuf.str();
		//std::string sc(std::istreambuf_iterator<char>(ifs),std::istreambuf_iterator<char>());
		//std::string sContent = sc;

		if (sContent.length()>0)
		{
			size_t index = sContent.find(L"FILEVERSION ");
			size_t indexEnd = -1;
			if (index != -1)
			{
				for(int i=index+12;i<sContent.length();i++)
				{
					if (sContent[i] == ' ') continue;
					if (sContent[i] == ',') index = i;
					if (sContent[i] == '\r' || sContent[i] == '\n')
					{
						indexEnd = i;
						break;
					}
				}
				if (indexEnd != -1) {
					std::wstring sEnd = sContent.substr(indexEnd);
					std::wstring sBegin = sContent.substr(0,index+1);
					//std::wstring sBegin = sContent.replace(index+1,indexEnd,L"$WCREV$");
					sContent=sBegin+L"$WCREV$"+sEnd;
				}
			}

			index = sContent.find(L"FileVersion\", \"");
			indexEnd = -1;
			if (index != -1)
			{
				for(int i=index+15;i<sContent.length();i++)
				{
					if (sContent[i] == ' ') continue;
					if (sContent[i] == '.' || sContent[i] == ',') index = i;
					if (sContent[i] == '"' || sContent[i] == '\\')
					{
						indexEnd = i;
						break;
					}
				}

				if (indexEnd != -1) {
					std::wstring sEnd = sContent.substr(indexEnd);
					std::wstring sBegin = sContent.substr(0,index+1);
					//std::wstring sBegin = sContent.replace(index+1,indexEnd,L"$WCREV$");
					sContent=sBegin+L"$WCREV$"+sEnd;
				}
			}

			fp = fopen(sRcFileTpl.c_str(),"wb+");
			if (fp)
			{
				std::string sOut = W2AString(sContent.c_str());
				fwrite(sOut.c_str(),1,sOut.length(),fp);
				fclose(fp);
			}
		}
	}
	return 0;
}

