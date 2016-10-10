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
	//���û�ͼʱ���˲�ģʽΪ�����������  
	g.SetSmoothingMode(SmoothingModeAntiAlias);  

	//����һ����ɫ�Ļ���  
	Pen *pen = new Pen(pens,1);  

	//����������ı�  
	g.DrawLine(pen,x+OFFSET_X,y,x+width-OFFSET_X,y);  

	//����������ı�  
	g.DrawLine(pen,x+OFFSET_X,y+height,x+width-OFFSET_X,y+height);  

	//����������ı�  
	g.DrawLine(pen,x,y+OFFSET_Y,x,y+height-OFFSET_Y);  

	//����������ı�  
	g.DrawLine(pen,x+width,y+OFFSET_Y,x+width,y+height-OFFSET_Y);  

	//���������Ͻǵ�Բ��  
	g.DrawArc(pen,x,y,OFFSET_X*2,OFFSET_Y*2,180,90);  

	//���������½ǵ�Բ��  
	g.DrawArc(pen,x+width-OFFSET_X*2,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,360,90);  

	//���������Ͻǵ�Բ��  
	g.DrawArc(pen,x+width-OFFSET_X*2,y,OFFSET_X*2,OFFSET_Y*2,270,90);  

	//���������½ǵ�Բ��  
	g.DrawArc(pen,x,y+height-OFFSET_Y*2,OFFSET_X*2,OFFSET_Y*2,90,90);  

	delete pen;  
}  

void DrawRoundRectange(Graphics &g,Color pens,LPRECT rc) 
{
	JKRect * pRc = (JKRect*)rc;
	DrawRoundRectange(g,pens,pRc->left,pRc->top,pRc->Width(),pRc->Height());
}

/* �����ɫ */  
void FillRoundRectangle(Graphics &g,Color color,int x,int y,int width,int height)  
{  
	//�������Ĳ��裺  
	//1����Բ�Ǿ��λ���Ϊ�ĸ�Բ���Ϸֳ��ĸ�ͬ�ȵ������������ֱ�Ǿ���  
	//2�����������ֱ�Ǿ���  
	//3��Ȼ������ĸ����ϵ�����  

	//������ˢ  
	Brush *brush = new SolidBrush(color);  

	//�������ֱ�Ǿ���  

	g.FillRectangle(brush,x,y+OFFSET_Y,width,height-OFFSET_Y*2);  
	g.FillRectangle(brush,x+OFFSET_X,y,width-OFFSET_X*2,height);  

	//����ĸ����ϵ�������  
	//������Ͻ�����  
#define PIE_OFFSET 2  
	g.FillPie(brush,x,y,OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,180,90);  

	//������½ǵ�����  
	g.FillPie(brush,x+width-(OFFSET_X*2+PIE_OFFSET),y+height-(OFFSET_Y*2+PIE_OFFSET),OFFSET_X*2+PIE_OFFSET,OFFSET_Y*2+PIE_OFFSET,360,90);  

	//������Ͻǵ�����  
	g.FillPie(brush,x+width-(OFFSET_X*2+PIE_OFFSET),y,(OFFSET_X*2+PIE_OFFSET),(OFFSET_Y*2+PIE_OFFSET),270,90);  

	//������½ǵ�����  
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
	//FontFamily fontFamily(L"����");
	//Font font(&fontFamily, Gdiplus::REAL(nFontSize), Gdiplus::FontStyleUnderline, Gdiplus::UnitPixel); 

	Gdiplus::StringFormat strformat;
	strformat.SetAlignment(Gdiplus::StringAlignmentCenter);
	strformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	strformat.SetTrimming(StringTrimmingEllipsisWord);

	g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit );  
	g.DrawString(A2WString(s.c_str()).c_str(),-1,&font,RectF(pRc->left,pRc->top,pRc->Width(),pRc->Height()),&strformat,&SolidBrush(color));
}