// MemoryLoad.cpp: implementation of the CMemoryLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryLoad.h"
#include "../ToolHelper/ToolOper.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace MEM
{
	
	FILE_HCUSTOMMODULE_MAP	 g_file_dll_map;
	
	HCUSTOMMODULE MyCustomLoadLibraryFunc(LPCSTR name, void * userdata)
	{
		
		HMEMORYMODULE hdll = NULL;
		
		CToolOper	oper;
		
		//加上绝对路径
		char	path[MAX_PATH];
		memset(path,0,sizeof(path));
		strncpy(path,(char*)userdata,MAX_PATH - 1);
		
		strcat(path,name);
		
		DWORD	len = 0;
		if(oper.ReadLocalFile(path,NULL,len) == ERROR_INSUFFICIENT_BUFFER)
		{
			char	*file_buffer = (char*)malloc(len);
			if(oper.ReadLocalFile(path,file_buffer,len) == NO_ERROR)
			{
				hdll = MemoryLoadLibrary(file_buffer);
			}
			else
			{
				free(file_buffer);
			}
		} 
		
		if (hdll == NULL)
		{
			hdll = (HMEMORYMODULE)LoadLibraryA(name);
		}
		else
		{
			g_file_dll_map[hdll] = 0;
		}
		
		return 	hdll;
	}
	
	FARPROC MyCustomGetProcAddressFunc(HCUSTOMMODULE hdll, LPCSTR name, void * userdata)
	{
		FARPROC ret_fun = NULL;
		
		FILE_HCUSTOMMODULE_MAP::iterator iter = g_file_dll_map.find(hdll);
		
		if (iter == g_file_dll_map.end())
		{
			ret_fun = GetProcAddress((HMODULE) hdll, name);
		}
		else
		{
			ret_fun = MemoryGetProcAddress(hdll,name);
		} 
		return ret_fun;
	}
	
	void MyCustomFreeLibraryFunc(HCUSTOMMODULE hdll, void * userdata)
	{
		
		FILE_HCUSTOMMODULE_MAP::iterator iter = g_file_dll_map.find(hdll);
		
		if (iter == g_file_dll_map.end())
		{
			FreeLibrary((HMODULE) hdll);
		}
		else
		{
			MemoryFreeLibrary(hdll);
		}
		
		return ;
	}
	
	HMEMORYMODULE MyMemoryLoadLibraryEx(const void *buffer,void * userdata)
	{
		return MemoryLoadLibraryEx(buffer,MyCustomLoadLibraryFunc,MyCustomGetProcAddressFunc,
			MyCustomFreeLibraryFunc,userdata);
	}
}