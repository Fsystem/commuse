#ifndef __CUString_H
#define __CUString_H

/** ���õ��ַ�������

*   FileName  : CUString.h

*   Author    : Double Sword

*   date      : 2011-8-8

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

namespace COMMUSE
{
	class CUString
	{
	public:

		//���þ�̬����
	public:
		//�Զ���ȡ�����ַ��������
		//(�磺AutoFillString(�����123456789,4,".")==>���...)
		/** 
		*   @Author : Double sword
		*   @Params : str������Ĵ�,nLenѭ����ʾ�ĳ���,	chReplaceʣ�µ��滻
		*   @Return : 
		*   @Date   : 2011-8-8
		*/
		static std::string AutoFillString(std::string& str,int nLen,const char* chReplace);

	protected:
	private:
	};
}

#endif //__CUString_H