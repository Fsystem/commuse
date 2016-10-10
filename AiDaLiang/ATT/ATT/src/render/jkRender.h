#ifndef __jkRender_H
#define __jkRender_H

//gdi+
#include <GdiPlus.h>
using namespace Gdiplus;

//-------------------------------------------------------------------------------
void SetupGdiPlus();
void UnstallGdiPlus();
//-------------------------------------------------------------------------------
JKRect GetRelativeRect(LPRECT rcParant,LPRECT rcChild);
void DrawRoundRectange(Graphics &g,Color pens,int x,int y,int width,int height);
void DrawRoundRectange(Graphics &g,Color pens,LPRECT rc);
void FillRoundRectangle(Graphics &g,Color color,int x,int y,int width,int height);
void FillRoundRectangle(Graphics &g,Color color,LPRECT rc);
void DrawText(Graphics &g,Font& font,std::string s,Color color,LPRECT rc);
//-------------------------------------------------------------------------------

#endif //__jkRender_H