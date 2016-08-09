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
//extern int gAnalysisMode ; //0����� 1���߳� 2�����̵��߳�

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
			//MessageBoxA(NULL,"������ϣ���ʼִ���ļ�д��","��ʾ",MB_OK);
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

					//ɾ����ʱ�ļ�
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
			sTmp += "\r\n��ʱ:";
			sTmp += std::to_string((LONGLONG)(GetTickCount()-dwCastTime));
			sTmp += "ms\r\n";
			sTmp += "ȫ�����,�õ��Ľ����:[";
			sTmp += std::to_string((LONGLONG)nMax);
			sTmp += "][���:RET.TXT]\r\n";

			Edit_SetText(GetDlgItem(gMainHwnd,IDC_EDIT_RESULT),A2TString(sTmp.c_str()).c_str());

			MessageBoxA(NULL,"�ļ�������д�����,��鿴RET.TXT��\r\n����ÿ���µĲ���ǰɾ������TXT�ļ�","��ʾ",MB_OK);

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

			MessageBoxA(NULL,"�������\r\nRET_CHILD.TXT-��ʾ��վ\r\nRET_PUBLISH.TXT-��ʾ����վ\r\nRET_ERROR.TXT-�������ӵ�url","��ܰ��ʾ",MB_OK);
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