#ifndef __jkFactory_H
#define __jkFactory_H
#include "jkInterface.h"


/**
 * @brief �Զ����ɹ��������
 */
#define JK_CREATE_FACTORY_ITEM(clazz,...) \
public:\
	virtual JKFactoryItemBase* CreateInstance()\
	{\
		return new clazz(__VA_ARGS__);\
	}

/**  
* @brief ���������������
*
* @baseclass IJKFactoryItem<JKFactoryItemBase> 
* @author Double Sword
* @data	2016-9-7
*   
* ��ϸ������
*/
class JKFactoryItemBase : public IJKFactoryItem<JKFactoryItemBase>
{
	JK_CREATE_FACTORY_ITEM(JKFactoryItemBase)
public:

protected:
private:
};

/**  
* @brief �๤��
*
* @baseclass ��
* @author Double Sword
* @data	2016-9-7
*   
* ��ϸ������
*/
template<class T>
class JKFactory
{
	std::list<void*> objectPool_;
public:
	/**
	* @brief  CreateInstance()
	* @param  ��			
	* @return ָ�����͵�ʵ��ָ��
	* @date	2016-9-7
	*
	* ��ϸ������������������
	*/
	T* CreateInstance();
	/**
	* @brief  ReleaseInstance( void* pRelease )
	* @param  pRelease ��Ҫ�ͷŵĹ�������			
	* @return void
	* @date	2016-9-7
	*
	* ��ϸ�������ͷŵĹ�������
	*/
	void ReleaseInstance(void* pRelease);
	/**
	* @brief  ���ٹ��� Destroy(  )
	* @param  void			
	* @return void
	* @date	2016-9-7
	*
	* ��ϸ���������ٹ���
	*/
	void Destroy();
};

//-------------------------------------------------------------------------------
// ʵ�ֲ���
//-------------------------------------------------------------------------------
template <class T>
T* JKFactory<T>::CreateInstance()
{
	JKFactoryItemBase* pItemNew = new JKFactoryItemBase;
	T* pT = pItemNew->CreateInstance();
	delete pItemNew;
	objectPool_->push_back(pT);
	return pT;
}

template <class T>
void JKFactory<T>::ReleaseInstance(void* pRelease)
{
	for (std::list<void*>::iterator it = objectPool_.begin();
		it!=objectPool_.end();it++)
	{
		if (pRelease == *it)
		{
			delete pRelease;
			objectPool_.erase(it);
			return;
		}
	}
}


template <class T>
void JKFactory<T>::Destroy()
{
	for (std::list<void*>::iterator it = objectPool_.begin();
		it!=objectPool_.end();it++)
	{
		delete it;
	}
	objectPool_.clear();
}
#endif //__jkFactory_H