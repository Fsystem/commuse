#include <string>
#include <atlconv.h>
#include "CUString.h"

namespace COMMUSE
{
	std::string CUString::AutoFillString(std::string& str,
		int nLen,const char* chReplace)
	{
		std::string sout = "";

		USES_CONVERSION;

		char szItem[64] = {0};
		wchar_t wszItem[2]=L"\0";
		std::wstring w = A2W(str.c_str());

		if (str.size() <= nLen)
		{
			sout = str;
		}
		else
		{
			for (int i = 0; i<w.size();i++)
			{
				wszItem[0] = w[i];
				sout += W2A(wszItem);
				if ( sout.size() >= nLen - 4 )
				{
					sout+=std::string(chReplace) +std::string(chReplace) +std::string(chReplace);
					break;
				}
			}
		}
		return sout;
	}
}