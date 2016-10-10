#include "stdafx.h"
#include "jkRender.h"

#pragma comment(lib,"GdiPlus.lib")



//-------------------------------------------------------------------------------
static ULONG_PTR gdiplusToken;  

void SetupGdiPlus()
{
	GdiplusStartupInput StartupInput;    
	GdiplusStartup(&gdiplusToken,&StartupInput,NULL);   
}

void UnstallGdiPlus()
{
	GdiplusShutdown(gdiplusToken);
}

//-------------------------------------------------------------------------------

static const int OFFSET_X = 0,OFFSET_Y=0;

JKRect GetRelativeRect(LPRECT rcParant,LPRECT rcChild)
{
	JKRect rc(rcParant->left+rcChild->left
		,rcParant->top+rcChild->top
		,rcChild->right-rcChild->left
		,rcChild->bottom-rcChild->top);

	return rc;

}

void DrawRoundRectange(Graphics &g,Color pens,int x,int y,int width,int height)  
{  
	//设置画图时的滤波模式为消除锯齿现象  
	g.SetSmoothingMode(SmoothingModeAntiAlias);  

	//创建一个红色的画笔  
	Pen *pen = new Pen(pens,1);  

	//画矩形上面的边  
	g.DrawLine(pen,x+OFFSET_X,y,x+width-OFFSET_X,y);  

	//画矩形下面的边  
	g.DrawLine(pen,x+OFFSET_X,y+height,x+width-OFFSET_X,y+height);  

	//画矩形左面的边  
	g.DrawLine(pen,x,y+OFFSET_Y,x,y+height-OFFSET_Y);  

	//画矩形右面的边  
	g.DrawLine(pen,x+width,y+OFFSET_Y,x+width,y+height-OFFSET_Y);  

	//画矩形左上角的圆角  
	g.DrawArc(pen,x,y,OFFSET_X*2,OFFSET_Y*2,180,90);  

	//画矩形右下角的圆角  
	g.DrawArc(pen,x+width-OFFSET_X*2,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,360,90);  

	//画矩形右上角的圆角  
	g.DrawArc(pen,x+width-OFFSET_X*2,y,OFFSET_X*2,OFFSET_Y*2,270,90);  

	//画矩形左下角的圆角  
	g.DrawArc(pen,x,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,90,90);  

	delete pen;  
}  

void DrawRoundRectange(Graphics &g,Color pens,LPRECT rc) 
{
	JKRect * pRc = (JKRect*)rc;
	DrawRoundRectange(g,pens,pRc->left,pRc->top,pRc->Width(),pRc->Height());
}

/* 填充颜色 */  
void FillRoundRectangle(Graphics &g,Color color,int x,int y,int width,int height)  
{  
	//矩形填充的步骤：  
	//1、把圆角矩形画分为四个圆角上分成四个同等的扇形外加三个直角矩形  
	//2、先填充三个直角矩形  
	//3、然后填充四个角上的扇形  

	//创建画刷  
	Brush *brush = new SolidBrush(color);  

	//填充三个直角矩形  

	g.FillRectangle(brush,x,y+OFFSET_Y,width,height-OFFSET_Y*2);  
	g.FillRectangle(brush,x+OFFSET_X,y,width-OFFSET_X*2,height);  

	//填充四个角上的扇形区  
	//填充左上角扇形  
#define PIE_OFFSET 2  
	g.FillPie(brush,x,y,OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,180,90);  

	//填充右下角的扇形  
	g.FillPie(brush,x+width-(OFFSET_X*2+PIE_OFFSET),y+height-(OFFSET_Y*2+PIE_OFFSET),OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,360,90);  

	//填充右上角的扇形  
	g.FillPie(brush,x+width-(OFFSET_X*2+PIE_OFFSET),y,(OFFSET_X*2+PIE_OFFSET),(OFFSET_Y*2+PIE_OFFSET),270,90);  

	//填充左下角的扇形  
	g.FillPie(brush,x,y+height-(OFFSET_X*2+PIE_OFFSET),(OFFSET_X*2+PIE_OFFSET),(OFFSET_Y*2+PIE_OFFSET),90,90);  
	delete brush;  
}  

void FillRoundRectangle(Graphics &g,Color color,LPRECT rc)
{
	JKRect * pRc = (JKRect*)rc;
	FillRoundRectangle(g,color,pRc->left,rc->top,pRc->Width(),pRc->Height());
}

//
void DrawText(Graphics &g,Font& font,std::string s,Color color,LPRECT rc)
{
	JKRect * pRc = (JKRect*)rc;
	//FontFamily fontFamily(L"宋体");
	//Font font(&fontFamily, Gdiplus::REAL(nFontSize), Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel); 

	Gdiplus::StringFormat strformat;
	strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
	strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	strformat.SetTrimming(StringTrimmingEllipsisWord);

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit );  
	g.DrawString(A2WString(s.c_str()).c_str(),-1,&font,RectF(pRc->left,pRc->top,pRc->Width(),pRc->Height()),&strformat,&SolidBrush(color));
}