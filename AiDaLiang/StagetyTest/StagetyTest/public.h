#ifndef _ADL_PUBLIC_DEFS_H_
#define _ADL_PUBLIC_DEFS_H_



void PopTipMessage(LPCSTR pTitle, LPCSTR pMsgStart, LPCSTR pMsgEnd, int nTipTime);

HMODULE ThisModuleHandle();

bool RestartMyComputer();
bool ShutDownMyComputer();


#ifndef _NOLOG
//#include "LogThread.h"
#endif

#endif

