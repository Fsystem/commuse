#ifndef __jkFactory_H
#define __jkFactory_H
#include "jkInterface.h"


/**
 * @brief 自动生成工厂对象宏
 */
#define JK_CREATE_FACTORY_ITEM(clazz,...) \
public:\
	virtual JKFactoryItemBase* CreateInstance()\
	{\
		return new clazz(__VA_ARGS__);\
	}

/**  
* @brief 创建工厂对象基类
*
* @baseclass IJKFactoryItem<JKFactoryItemBase> 
* @author Double Sword
* @data	2016-9-7
*   
* 详细描述：
*/
class JKFactoryItemBase : public IJKFactoryItem<JKFactoryItemBase>
{
	JK_CREATE_FACTORY_ITEM(JKFactoryItemBase)
public:

protected:
private:
};

/**  
* @brief 类工厂
*
* @baseclass 无
* @author Double Sword
* @data	2016-9-7
*   
* 详细描述：
*/
template<class T>
class JKFactory
{
	std::list<void*> objectPool_;
public:
	/**
	* @brief  CreateInstance()
	* @param  无			
	* @return 指定类型的实例指针
	* @date	2016-9-7
	*
	* 详细描述：创建工厂对象
	*/
	T* CreateInstance();
	/**
	* @brief  ReleaseInstance( void* pRelease )
	* @param  pRelease 需要释放的工厂对象			
	* @return void
	* @date	2016-9-7
	*
	* 详细描述：释放的工厂对象
	*/
	void ReleaseInstance(void* pRelease);
	/**
	* @brief  销毁工厂 Destroy(  )
	* @param  void			
	* @return void
	* @date	2016-9-7
	*
	* 详细描述：销毁工厂
	*/
	void Destroy();
};

//-------------------------------------------------------------------------------
// 实现部分
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