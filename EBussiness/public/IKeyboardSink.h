#ifndef __IKeyboardSink_H
#define __IKeyboardSink_H
/** keyboard callback sink

*   FileName  : IKeyboardSink.h

*   Author    : Double Sword

*   date      : 2012-3-21

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

struct IKeyboardSink
{
	virtual void ScanGunCallBackData(char* pszData,int nNum)=0; 
};
#endif //__IKeyboardSink_H