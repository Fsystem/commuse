#ifndef __PoolMemData_H
#define __PoolMemData_H
/**
 * @brief �ڴ����
 *
 * ��ϸ������
 */
class CPoolMemData
{
	typedef	std::list<PVOID>			IO_LIST;		
	typedef	std::map<DWORD,IO_LIST>		IO_KEY_LIST;	//��������о��,KEYΪ�ڴ��Ĵ�С
public:
	CPoolMemData(void);
	~CPoolMemData(void);
public:  

	PVOID			GetBuffer(DWORD size);
	void			FreeBuffer(PVOID p,DWORD size);
	void			FreeAll();

private:

	yCMyseclock		m_data_lock;
	IO_KEY_LIST		m_io_map;
};
#endif //__PoolMemData_H


