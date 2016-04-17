/** 策略集合
*   @FileName  : StagetyContext.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-8
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __StagetyContext_H
#define __StagetyContext_H

/*! 进程策略 */
class StagetyProcess : public IStagety
{
	//实现接口
public:
	virtual ActionType GetActionType();
	virtual OperateType GetOperateType();
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName); //处理驱动 0-信任 其他为不信任
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child);//处理进程 0-信任 其他为不信任

};

/*! 驱动策略 */
class StagetyDriver : public IStagety
{
	//实现接口
public:
	virtual ActionType GetActionType();
	virtual OperateType GetOperateType();
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName); //处理驱动 0-信任 其他为不信任
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child);//处理进程 0-信任 其他为不信任

};

#endif //__StagetyContext_H