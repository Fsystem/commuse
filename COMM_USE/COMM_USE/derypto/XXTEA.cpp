//#include "stdafx.h"
#include "XXTEA.h"

//Encode function
void XXTEA::DecryptTea(unsigned long *firstChunk, unsigned long *secondChunk , 
	const char* const pKey)
{
	if (firstChunk && secondChunk && pKey)
	{
		unsigned long *key   = (unsigned long *)pKey;
		unsigned long  sum = 0;
		unsigned long  y     = *firstChunk;
		unsigned long  z     = *secondChunk;
		unsigned long  delta = 0x9e3779b9;
		sum = delta << 5;
		for(int i=0; i<32; i++) 
		{
			z   -= (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3];
			y   -= (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
			sum -= delta;
		}
		*firstChunk  = y;
		*secondChunk = z;
	}
}
//-------------------------------------------------------------------------------
void XXTEA::EncryptTea(unsigned long *firstChunk, unsigned long *secondChunk , 
	const char* const pKey) 
{
	if (firstChunk && secondChunk && pKey)
	{
		unsigned long y=*firstChunk;
		unsigned long z=*secondChunk;
		unsigned long sum=0;

		unsigned long *key   = (unsigned long *)pKey;

		unsigned long delta=0x9e3779b9;

		for (int i=0; i < 32; i++)
		{                            
			sum += delta;
			y += ((z<<4) + key[0]) ^ (z + sum) ^ ((z>>5) + key[1]);
			z += ((y<<4) + key[2]) ^ (y + sum) ^ ((y>>5) + key[3]); 
		}

		*firstChunk  = y; 
		*secondChunk = z;
	}
	
}

//-------------------------------------------------------------------------------
void XXTEA::DecryptBuffer(char* buffer, int size , const char* const pKey)
{
	if (buffer && pKey)
	{
		char *p = buffer;
		while (p < buffer + size && p + sizeof(unsigned long) * 2 <= buffer + size ) 
		{
			DecryptTea((unsigned long *)p, (unsigned long *)(p + sizeof(unsigned long)),pKey);
			p += sizeof(unsigned long) * 2;
		} 
	}
}
//-------------------------------------------------------------------------------
void XXTEA::EncryptBuffer(char* buffer, int size , const char* const pKey)
{
	if (buffer && pKey)
	{
		char *p = buffer;
		while (p < buffer + size && p + sizeof(unsigned long) * 2 <= buffer + size ) 
		{
			EncryptTea((unsigned long *)p, (unsigned long *)(p + sizeof(unsigned long)) , pKey);
			p += sizeof(unsigned long) * 2;
		} 
	}
}
//-------------------------------------------------------------------------------
