#ifndef __CUString_H
#define __CUString_H

/** 常用的字符串处理

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

		//常用静态函数
	public:
		//自动截取有限字符串，填充
		//(如：AutoFillString(你好吗123456789,4,".")==>你好...)
		/** 
		*   @Author : Double sword
		*   @Params : str待处理的串,nLen循序显示的长度,	chReplace剩下的替换
		*   @Return : 
		*   @Date   : 2011-8-8
		*/
		static std::string AutoFillString(std::string& str,int nLen,const char* chReplace);

	protected:
	private:
	};
}

#endif //__CUString_H