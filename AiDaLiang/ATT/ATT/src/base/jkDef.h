#ifndef __jkDef_H
#define __jkDef_H

enum MenuID
{
	IDR_MenuId_Tool = 9000,
	IDR_MenuId_Tool_Md5,
	IDR_MenuId_Tool_Reg,
	IDR_MenuId_Tool_Exit,

	IDR_MenuId_Custom_Base=10000,
};


//ºêº¯Êý
static POINT makePoint(int x,int y) {POINT pt;pt.x = x;pt.y=y;return pt;}

#endif //__jkDef_H