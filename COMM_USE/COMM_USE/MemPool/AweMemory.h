/************************************************************************/
/* �ڴ��Դ��                                                            */
/*���ߣ�˫�н�															*/
/*ʱ��:2010-10-21														*/
/************************************************************************/

#pragma once

void SetPageSize(DWORD dwSize);
BOOL MemCreate(DWORD dwMemSize);
void MemDestroy(void);

/* �ṹ���� */
typedef struct _MemList
{
	void*				_data;
	struct _MemList*	_pNext;
}MemList,PMemList;

extern MemList*	pMemlistHead	;


