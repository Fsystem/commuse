#ifndef __jkInterface_H
#define __jkInterface_H
#include "jkDef.h"
#include <list>
#include <vector>

#define Interface struct 

#define MAX_MENUNAME_LEN 32

//-------------------------------------------------------------------------------
//业务数据
//-------------------------------------------------------------------------------

/**
 * @brief 菜单数据
 *
 * 详细描述：
 */

Interface IJKMenu;


/**
 * @brief 菜单数据
 *
 * 详细描述：和dll通信的菜单数据项
 */
struct JKMenuData 
{
	char szMenuName[MAX_MENUNAME_LEN]; ///< 菜单名称
	IJKMenu* pCallPtr;
	bool (IJKMenu::*MenuCall)(const char* szMenuName,HWND hClientMain);
	void (IJKMenu::*ReveryfySize)(const char* szMenuName);
	void (IJKMenu::*CloseMenu)(const char* szMenuName);
	//JKMenuData* subMenu; ///< 子菜单数据，如果为null表示没有子菜单

	//bool (IJKMenu::*MenuCallback)(HWND hMainWnd); ///< 菜单回调函数
};

/**
 * @brief 菜单项目数组指针，以NULL作为数据的结束
 *
 * 详细描述：菜单项目数组指针，以NULL作为数据的结束
 */
typedef JKMenuData * JKMenuDataArr ,** LPJKMenuDataArr;

//-------------------------------------------------------------------------------
//业务类
//-------------------------------------------------------------------------------
/**
 * @brief 工厂元素接口
 *
 * 详细描述：
 */
template < class T>
Interface IJKFactoryItem 
{
	/**
	 * @brief 创建实例接口
	 *
	 * 详细描述：
	 */
	virtual T* CreateInstance() = 0;
};

Interface IJKMenu
{
	virtual HWND GetMainHwd() = 0;
	virtual LPJKMenuDataArr LoadMenu() = 0;
};

#define JKMENU_CreateInstance(h,jkMenu)\
{\
	typedef IJKMenu* (*PFN_CreateMenuInstance)();\
	PFN_CreateMenuInstance fn = (PFN_CreateMenuInstance)GetProcAddress(h,"CreateMenuInstance");\
	jkMenu = fn();\
}


#define JK_MENU_EXPORT_INSTANCE(claszzPointer) \
extern "C" __declspec(dllexport) IJKMenu* CreateMenuInstance(){\
	if(claszzPointer != NULL)\
		return reinterpret_cast<IJKMenu*> ( claszzPointer );\
	else\
		return NULL;\
}

//-------------------------------------------------------------------------------
//辅助类
//-------------------------------------------------------------------------------

/**
 * @brief 矩形类
 *
 * 详细描述：继承RECT，添加部分函数
 */
struct JKRect:public RECT
{
	JKRect(){}
	JKRect(int x,int y,int w,int h){left=x;top=y;right=left+w;bottom=top+h;}
	JKRect(RECT rc){*(RECT*)this = rc;}
	int Width()const {return right-left;}
	int Height()const {return bottom-top;}
	POINT Center() const {return makePoint(left+Width()/2,top+Height()/2);}
	POINT Start() const { return makePoint(left,top); }
	POINT End() const {return makePoint(right,bottom);}
	JKRect Scale(int dw,int dh) {JKRect rcOut(left+dw/2,top+dh/2,right-left-dw,bottom-top-dh);return rcOut;}

	//-------------------------------------------------------------------------------
	static JKRect ZERO(){return JKRect(0,0,0,0);}
};
#endif //__jkInterface_H