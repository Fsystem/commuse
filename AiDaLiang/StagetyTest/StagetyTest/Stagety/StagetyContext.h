/** ���Լ���
*   @FileName  : StagetyContext.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-4-8
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __StagetyContext_H
#define __StagetyContext_H

/*! ���̲��� */
class StagetyProcess : public IStagety
{
	//ʵ�ֽӿ�
public:
	virtual ActionType GetActionType();
	virtual OperateType GetOperateType();
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName); //�������� 0-���� ����Ϊ������
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child);//������� 0-���� ����Ϊ������

};

/*! �������� */
class StagetyDriver : public IStagety
{
	//ʵ�ֽӿ�
public:
	virtual ActionType GetActionType();
	virtual OperateType GetOperateType();
	virtual int HandleDriver(LPCSTR pOldPath, LPCSTR pDriverName); //�������� 0-���� ����Ϊ������
	virtual int HandleProcess(ActionType action,OperateType optType, ProcessInfoStagety* parant, ProcessInfoStagety* child);//������� 0-���� ����Ϊ������

};

#endif //__StagetyContext_H