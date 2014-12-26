#ifndef __EKeyHook_H
#define __EKeyHook_H
/** global keyboard sink module

*   FileName  : EKeyHook.h

*   Author    : Double Sword

*   date      : 2012-3-21

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

#include "IKeyboardSink.h"

#ifdef EKEYHOOK_EXPORTS
#define EKEYHOOK_API __declspec(dllexport)
#else
#define EKEYHOOK_API __declspec(dllimport)
#endif


/** this class was exported from "EKeyHook.dll"

*   ClassName	: CEKeyHook

*   Author	: Double Sword

*   Data	: 2012-3-21

*/
class EKEYHOOK_API CEKeyHook {
public:
	static CEKeyHook* CreateInstance(){
		if (!m_pThis)
		{
			m_pThis=new CEKeyHook;
		}
		return m_pThis;
	}
	~CEKeyHook();
private:
	CEKeyHook(void);
	

	//operate
public:

	/** Start keyboard sink
	*   @Author : Double sword
	*   @Params : 	
	*   @Return : success return true,else false
	*   @Date   : 2012-3-21
	*/
	BOOL StartMaskKeyboard();

	/** Stop keyboard sink
	*   @Author : Double sword
	*   @Params : 		
	*   @Return : 
	*   @Date   : 2012-3-21
	*/
	void StopMaskKeyboard();

	/** set key callback sink
	*   @Author : Double sword
	*   @Params : 		
	*   @Return : 
	*   @Date   : 2012-3-21
	*/
	void SetKeyCallBackSink(IKeyboardSink* pSink);

	//private data
private:
	HHOOK			m_hHook;	//HOOK HANDLE
	IKeyboardSink*	m_pKeySink;	//Key sink pointer
	char			m_szKeyCode[MAX_PATH];//key code
	int				m_KeyCount;	//key count
	DWORD			m_dwTimeStamp;//key input time sum

	//private functions
private:
	void init();
	static CEKeyHook* m_pThis;
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void ThreadFunc(void* p);
};

extern EKEYHOOK_API int nEKeyHook;

EKEYHOOK_API int fnEKeyHook(void);


#endif //__EKeyHook_H