#ifndef __jkInterface_H
#define __jkInterface_H
#include "jkDef.h"
#include <list>
#include <vector>

#define Interface struct 

#define MAX_MENUNAME_LEN 32

//-------------------------------------------------------------------------------
//ҵ������
//-------------------------------------------------------------------------------

/**
 * @brief �˵�����
 *
 * ��ϸ������
 */

Interface IJKMenu;


/**
 * @brief �˵�����
 *
 * ��ϸ��������dllͨ�ŵĲ˵�������
 */
struct JKMenuData 
{
	char szMenuName[MAX_MENUNAME_LEN]; ///< �˵�����
	IJKMenu* pCallPtr;
	bool (IJKMenu::*MenuCall)(const char* szMenuName,HWND hClientMain);
	void (IJKMenu::*ReveryfySize)(const char* szMenuName);
	void (IJKMenu::*CloseMenu)(const char* szMenuName);
	//JKMenuData* subMenu; ///< �Ӳ˵����ݣ����Ϊnull��ʾû���Ӳ˵�

	//bool (IJKMenu::*MenuCallback)(HWND hMainWnd); ///< �˵��ص�����
};

/**
 * @brief �˵���Ŀ����ָ�룬��NULL��Ϊ���ݵĽ���
 *
 * ��ϸ�������˵���Ŀ����ָ�룬��NULL��Ϊ���ݵĽ���
 */
typedef JKMenuData * JKMenuDataArr ,** LPJKMenuDataArr;

//-------------------------------------------------------------------------------
//ҵ����
//-------------------------------------------------------------------------------
/**
 * @brief ����Ԫ�ؽӿ�
 *
 * ��ϸ������
 */
template < class T>
Interface IJKFactoryItem 
{
	/**
	 * @brief ����ʵ���ӿ�
	 *
	 * ��ϸ������
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
//������
//-------------------------------------------------------------------------------

/**
 * @brief ������
 *
 * ��ϸ�������̳�RECT����Ӳ��ֺ���
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