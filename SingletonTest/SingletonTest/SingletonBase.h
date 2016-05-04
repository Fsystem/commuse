/** 单例模板类，单例的基类
*   @FileName  : SingletonBase.h
*   @Author    : Double Sword(doublesword.jack@gmail.com)
*   @date      : 2016-5-4
*   @Copyright : Copyright belong to Author and 9u2,CO.LTD
*   @RefDoc    : --
*/

#ifndef __SingletonBase_H
#define __SingletonBase_H
#include <typeinfo.h>

template<class T>
class SingletonBase
{
public:
	/** 子类可以直接创建实例
	*   @Author   : Double sword
	*   @Params   : 
	*   @Return   :				
	*   @Date     :	2016-5-4
	*/
	static T& Instance()
	{
		static T t;

		if (t.szClassName == NULL)
		{
			t.szClassName=typeid(T).name();
		}

		return t;
	}

	/** 
	*   @Author   : Double sword
	*   @Params   : void
	*   @Return   :	class name string			
	*   @Date     :	2016-5-4
	*/
	const char* GetClassName()
	{
		return szClassName;
	}

protected:
	SingletonBase(){szClassName=NULL;}
	~SingletonBase(){}
protected:
	char const * szClassName;
};
#endif //__SingletonBase_H