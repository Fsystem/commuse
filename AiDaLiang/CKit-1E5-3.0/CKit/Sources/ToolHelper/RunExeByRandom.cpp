#include "StdAfx.h"
#include "RunExeByRandom.h"

RunExeByRandom::RunExeByRandom(LPCSTR lpszOringinName)
{
	mRandName.clear();
	mOringinName.clear();

	if(lpszOringinName) mOringinName = lpszOringinName;
}

BOOL RunExeByRandom::RandomCopySelf()
{
	CToolOper tp ;
	std::string sRandName = tp.GetTempName("QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm",".exe");

	char szPath[MAX_PATH],szAppDir[MAX_PATH];
	memset(szPath,0,sizeof szPath);
	memset(szAppDir,0,sizeof szAppDir);

	strncpy(szPath,tp.GetAppPath(),MAX_PATH-1);
	strncpy(szAppDir,szPath,MAX_PATH-1);

	char* pFind = strrchr(szAppDir,'\\');
	if (pFind)
	{
		pFind[1] = 0;
	}

	mRandName = szAppDir;
	mRandName += sRandName;

	return CopyFileA(szPath,mRandName.c_str(),FALSE);
}

void RunExeByRandom::RunNewExe(LPCSTR lpszPath,LPCSTR lpszCmd)
{
	STARTUPINFOA si={sizeof STARTUPINFOA};
	PROCESS_INFORMATION pi;

	//char szExeCmd[4096]={0};
	//sprintf(szExeCmd,"%s %s",lpszPath,lpszCmd);
	CreateProcessA(lpszPath,(LPSTR)lpszCmd,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
}

void RunExeByRandom::DeleteOldExe(LPCSTR lpszPath)
{
	DeleteFileA(lpszPath);
}

bool RunExeByRandom::Run()
{
	if(mOringinName.length()==0) return false;

	CToolOper tp ;

	char szPath[MAX_PATH];
	memset(szPath,0,sizeof szPath);

	strncpy(szPath,tp.GetAppPath(),MAX_PATH-1);

	char* pFind = strrchr(szPath,'\\');
	if (pFind == NULL) return false;

	pFind +=1;

	std::string sCmdLine="";
	for (int i=0;i<__argc;i++)
	{
		sCmdLine += T2AString(__targv[i]);
		sCmdLine += " ";
	}

	if(sCmdLine.length()>0) sCmdLine.erase(sCmdLine.length()-1);

	if (stricmp(pFind,mOringinName.c_str()) == 0)
	{
		if(RandomCopySelf())
		{
			RunNewExe(mRandName.c_str(),sCmdLine.c_str());
		}

		return true;
	}
	else
	{
		if(__argc>0) DeleteOldExe(T2AString(__targv[0]).c_str());
	}

	return false;
}