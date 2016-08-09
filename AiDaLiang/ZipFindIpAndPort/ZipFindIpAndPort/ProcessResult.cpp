#include "stdafx.h"
#include "ProcessResult.h"
#include <process.h>
//-------------------------------------------------------------------------------
extern char szAppDir[MAX_PATH]		;
extern char szRetFile[MAX_PATH]		;
extern DWORD dwCastTime				;
//extern char szDownZipDir[MAX_PATH]	;
//extern char szDownZipConfig[MAX_PATH];
//
//extern HWND gMainHwnd;
//extern HWND gParantHwnd;
//extern int gFileIndex ;
//extern char gszZipFile[4096] ;
//extern char gszKeys[4096] ;
//extern int gAnalysisMode ; //0多进程 1多线程 2单进程单线程

bool bAnalysisFinish = false;
bool bFilterFinish =false;
std::list<IPC_Data> listIpcDatas;
std::map<int,std::list<std::string>> mapFilterResult;
//-------------------------------------------------------------------------------

void ProcessResultThread(void* p)
{
	while(1)
	{
		if (bAnalysisFinish)
		{
			//MessageBoxA(NULL,"解析完毕，开始执行文件写入","提示",MB_OK);
			int nPos = 0,nMax = listIpcDatas.size();
			SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS_RESULT),PBM_SETPOS,nPos,0);
			std::map<std::string,int> mapUrls;
			for (auto it = listIpcDatas.begin();it != listIpcDatas.end();it++)
			{
				if (it->bProcessed)
				{
					{
						std::ifstream infile(it->szRetFile);
						std::string sLine;
						while(std::getline(infile,sLine,'\n'))
						{
							if(sLine.length()==0) break;
							mapUrls[sLine] = 0;
						}
					}

					//删除临时文件
					DeleteFileA(it->szRetFile);
				}

				SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS_RESULT),PBM_SETPOS,(++nPos)*50/nMax,0);
			}

			nPos = 0;
			nMax = mapUrls.size();

			for (auto it = mapUrls.begin();it != mapUrls.end();it++)
			{
				std::ofstream outfile(szRetFile,std::ios::app);
				outfile << it->first<<std::endl;

				SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS_RESULT),PBM_SETPOS,50+(++nPos)*50/nMax,0);
			}

			SendMessage(GetDlgItem(gMainHwnd,IDC_PROGRESS_RESULT),PBM_SETPOS,100,0);

			TCHAR szContent[4096];
			Edit_GetText(GetDlgItem(gMainHwnd,IDC_EDIT_RESULT),szContent,_countof(szContent));
			std::string sTmp = T2AString(szContent);
			sTmp += "\r\n耗时:";
			sTmp += std::to_string((LONGLONG)(GetTickCount()-dwCastTime));
			sTmp += "ms\r\n";
			sTmp += "全部完成,得到的结果数:[";
			sTmp += std::to_string((LONGLONG)nMax);
			sTmp += "][结果:RET.TXT]\r\n";

			Edit_SetText(GetDlgItem(gMainHwnd,IDC_EDIT_RESULT),A2TString(sTmp.c_str()).c_str());

			MessageBoxA(NULL,"文件解析和写入完成,请查看RET.TXT。\r\n建议每次新的操作前删除所有TXT文件","提示",MB_OK);

			EnableWindow(GetDlgItem(gMainHwnd,IDOK),TRUE);

			listIpcDatas.clear();

			bAnalysisFinish = false;
		}

		Sleep(1000);
	}
}

void ProcessFilterThread(void* p)
{
	while(1)
	{
		if (bFilterFinish)
		{
			bFilterFinish = false;

			std::string szTmp;
			std::ofstream outchldfile;
			std::ofstream outpubfile;
			std::ofstream outerrfile;
			
			szTmp = szAppDir+std::string("RET_CHILD.TXT");
			outchldfile.open(szTmp,std::ios::out);
			szTmp = szAppDir+std::string("RET_PUBLISH.TXT");
			outpubfile.open(szTmp,std::ios::out);
			szTmp = szAppDir+std::string("RET_ERROR.TXT");
			outerrfile.open(szTmp,std::ios::out);

			for (auto it = mapFilterResult.begin();it != mapFilterResult.end(); it++)
			{
				switch(it->first)
				{
				case 0:
					{
						std::for_each(it->second.begin(),it->second.end(),[&outchldfile](std::string sItem){outchldfile<<sItem<<std::endl;});
						break;
					}
				case 1:
					{
						std::for_each(it->second.begin(),it->second.end(),[&outpubfile](std::string sItem){outpubfile<<sItem<<std::endl;});
						break;
					}
				case -1:
					{
						std::for_each(it->second.begin(),it->second.end(),[&outerrfile](std::string sItem){outerrfile<<sItem<<std::endl;});
						break;
					}
				}
			}

			MessageBoxA(NULL,"过滤完成\r\nRET_CHILD.TXT-表示子站\r\nRET_PUBLISH.TXT-表示发布站\r\nRET_ERROR.TXT-不能连接的url","温馨提示",MB_OK);
			EnableWindow(gMainHwnd,TRUE);
		}
		Sleep(1000);
	}
}

//-------------------------------------------------------------------------------
void StartProcessResultService()
{
	_beginthread(ProcessResultThread,0,0);
	_beginthread(ProcessFilterThread,0,0);
}