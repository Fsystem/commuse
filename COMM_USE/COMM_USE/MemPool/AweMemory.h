/************************************************************************/
/* 内存池源码                                                            */
/*作者：双刃剑															*/
/*时间:2010-10-21														*/
/************************************************************************/

#pragma once

void SetPageSize(DWORD dwSize);
BOOL MemCreate(DWORD dwMemSize);
void MemDestroy(void);

/* 结构定义 */
typedef struct _MemList
{
	void*				_data;
	struct _MemList*	_pNext;
}MemList,PMemList;

extern MemList*	pMemlistHead	;


