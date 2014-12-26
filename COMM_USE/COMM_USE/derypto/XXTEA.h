/** TEAº”√‹Ω‚√‹
*   FileName  : XXTEA.h
*   Author    : Double Sword
*   date      : 2011-3-24
*   Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   RefDoc    : Œﬁ
*/
//encode.h file
#ifndef __ENCODE__H__
#define __ENCODE__H__
class XXTEA
{
public:
	//Encode function
	static void DecryptTea(unsigned long *firstChunk, unsigned long *secondChunk , const char* const pKey);
	static void EncryptTea(unsigned long *firstChunk, unsigned long *secondChunk , const char* const pKey);
	static void DecryptBuffer(char* buffer, int size , const char* const pKey );
	static void EncryptBuffer(char* buffer, int size , const char* const pKey);
protected:
private:
};

#endif