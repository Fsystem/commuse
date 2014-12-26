#ifndef __CCEdit_H
#define __CCEdit_H
/** �༭��
*   @FileName  : CCEdit.h
*   @Author    : Double Sword
*   @date      : 2012-12-14
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#include "AppInc.h"

class ETextField:public CCTextFieldTTF,public CCTargetedTouchDelegate  
{  
	bool m_isPassWord;  

	bool m_isLowSide;//�Ƿ��ڵ�λ  
	bool m_isTouchHandled;//�Ƿ񱻵����  
	bool m_isTouchedFun;  
	CCObject* m_selectorTarget;  
	float m_keyboardOffset;  

	SEL_CallFunc m_selector;  
public:  
	ETextField():m_isLowSide(false),m_isTouchHandled(false),m_isPassWord(false),m_isTouchedFun(false),inputStringLengthLimit(-1),m_isMove(false){};//
	~ETextField(){};

	static ETextField * ETextFieldWithPlaceHolder(const char *placeholder, const CCSize& dimensions, CCTextAlignment alignment, const char *fontName, float fontSize);
	static ETextField * ETextFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize);  

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);  
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);  

	//CCLAYER  
	virtual void onEnter();  
	virtual void onExit();  

	//CCLABEL   
	void setDimensions(CCSize rect){m_tDimensions=rect;};  
	//CCTEXTFIELDTTF  
	virtual void setString(const char *text);  
	void setString(const char *label,CCTextAlignment Alignment);  
	//CCIMEDelegate  
	virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info);  
	virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info);  


	void parentLayerUpMove();  
	void parentLayerBackMove();  
	void setIsPassWord(bool display){m_isPassWord=display;};  

	void setTouchedEvent(CCObject* SelectorTarget,SEL_CallFunc selector,bool isTargeted);  
	void touchedEventExecut();  
	virtual void insertText(const char * text, int len);  
	size_t inputStringLengthLimit;  
	size_t getStringLengthLimit() const { return inputStringLengthLimit; }  
	void setStringLengthLimit(size_t val) { inputStringLengthLimit = val; }  



	//caoxinst 20120807  
	bool IsUpdate;  
	bool m_isMove; //�Ƿ��ƶ�  
	int FieldWidth;//������ʾ�ı���Ŀ��  
	CCPoint Front; //��ָ�ı���ľ���λ��  
	CCPoint Last;  //�����ı���ľ���λ��  
	int stringLen ; //�ַ����� һ�����ֳ���Ϊ1�� һ��Ӣ���ַ�����Ϊ1   
	int len;  //�����ַ�������  
	CCObject* pSend;  
	void setisMove(bool val) { m_isMove = val; stringLen = 0; len = 0; IsUpdate = false;}  
	void setPsend(CCObject* psend=NULL){ pSend = psend ;}  
	void setStringLen(int n){len = n;}  
	void setFieldWidth(int width){FieldWidth = width;}  
	void setFieldPos(CCPoint front,CCPoint last){ Front = front; Last = last;}  
	void FieldMove(string insertText);  
	void deleteBackward();  
	void setinput(string strs){  
		*m_pInputText = strs;  
	}  
	int flags ; //��win32�� �������ص���� ���н���  
};    

#endif //__CCEdit_H