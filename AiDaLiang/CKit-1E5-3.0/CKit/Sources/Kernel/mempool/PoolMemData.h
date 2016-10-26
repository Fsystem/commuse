#ifndef __PoolMemData_H
#define __PoolMemData_H
/**
 * @brief 内存池类
 *
 * 详细描述：
 */
class CPoolMemData
{
	typedef	std::list<PVOID>			IO_LIST;		
	typedef	std::map<DWORD,IO_LIST>		IO_KEY_LIST;	//保存的所有句柄,KEY为内存块的大小
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


