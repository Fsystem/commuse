/** ���̵���Ϊ������¼
*   @FileName  : ActionOperateRecord.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-8
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __ActionOperateRecord_H
#define __ActionOperateRecord_H
#include <map>
#include <list>

using std::map;
using std::list;

class ActionOperateRecord
{
	/*! <PID,ProcessorOperateResult> */
	typedef std::map<DWORD,std::list<ActionOperateResult>> MapOperate;
public:
	~ActionOperateRecord(){};
	static ActionOperateRecord& Instance(){static ActionOperateRecord u;return u;};

	//��¼���̲���,�����Ƿ�
	BOOL RecordProcessOperate(const ActionOperateResult & operate);

private:
	ActionOperateRecord(){};

private:
	MapOperate mOperateRecord;		//������Ϊ��¼
};
#endif //__ActionOperateRecord_H