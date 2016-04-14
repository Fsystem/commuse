#include <StdAfx.h>
#include "StagetyInclude.h"
#include "ActionOperateRecord.h"
#include "helper/FunctionHelper.h"

BOOL ActionOperateRecord::RecordProcessOperate(const ActionOperateResult & operate)
{
	DWORD dwKey = GetActionKey(operate.parantProcess.szProcessPath.c_str(),
		operate.childProcess.szProcessPath.c_str(),operate.operateType);

	if(dwKey == 0)
	{
		LOGEVEN(TEXT("error:²úÉúCRC´íÎó\n"));
		return FALSE;
	}

	mOperateRecord[dwKey].push_back(operate);

	return (mOperateRecord[dwKey].size()==1);
}