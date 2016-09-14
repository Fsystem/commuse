// ECREV_RC.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <boost/regex.h>
#include<stdio.h>
#include <algorithm>

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
		CopyFileA(sRcFile.c_str(),sRcFileTpl.c_str(),FALSE);

		FILE* fp = fopen(sRcFileTpl.c_str(),"rb");
		fseek(fp,0,SEEK_END);
		int nLen = ftell(fp);
		wchar_t* szData = new wchar_t[nLen/2+1];
		rewind(fp);
		fread(szData,sizeof(wchar_t),nLen/2,fp);
		fclose(fp);
		szData[nLen/2] = 0;
		//std::wifstream ifs();
		//std::wstringstream sbuf;
		//if (ifs.is_open())
		//{
		//	//std::wstring wLine;
		//	//std::getline(ifs,wLine);

		//	sbuf<<ifs.rdbuf();
		//	ifs.close();
		//}
		//std::wstring wsContent = sbuf.str();
		//wsContent.erase(0,2);
		std::wstring sContent=szData;
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

			fp = fopen(sRcFileTpl.c_str(),"wb+,ccs=UNICODE");
			if (fp)
			{
				fwrite(sContent.c_str(),sizeof(wchar_t),sContent.length(),fp);
				fclose(fp);
			}
		}
	}
	return 0;
}

