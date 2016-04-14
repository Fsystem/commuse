#include <StdAfx.h>
#include "StagetyInclude.h"
#include "ActionOperateRecord.h"

BOOL ActionOperateRecord::RecordProcessOperate(const ActionOperateResult & operate)
{
	if(operate.actionType == enActionNull || operate.operateType == enOperateNull) return FALSE;

	char szRandKey[1024]={0};
	sprintf(szRandKey,"%s%s%d",operate.parantProcess.szProcessPath.c_str(),
		operate.childProcess.szProcessPath.c_str(),operate.operateType);

	CCRC crc;
	DWORD dwKey = crc.GetStrCrc_Key(szRandKey,strlen(szRandKey));

	mOperateRecord[dwKey].push_back(operate);

	return (mOperateRecord[dwKey].size()==1);
}