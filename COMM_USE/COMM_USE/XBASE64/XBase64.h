/*

** File Name: xbase64.h,xbase64.cpp

** Description: base64 º”√‹Ω‚√‹

** Author: À´»–Ω£

** Date : 2010.11.3

*/

namespace COMMUSE
{

	class XBase64
	{
	public:
		void Encode(char *src, int src_len, char *dst);
		void Decode(char *src, int src_len, char *dst);
		int HexToStr(const char *hex, char *str);
		int StrToHex(const char *str, char *hex , unsigned int nLength);

	};
}


 