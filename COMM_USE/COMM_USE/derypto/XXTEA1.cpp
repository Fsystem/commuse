#pragma once
#include "stdafx.h"
#include <Windows.h>
#include "XXTEA.h"
#pragma warning(disable:4996)
XXTEA::XXTEA(void)
{
}

XXTEA::~XXTEA(void)
{
}



int XXTEA::toIntArray(int ** ppresult, const unsigned char * pData, unsigned nDataLen, int includeLength)
{
	if (ppresult && pData && nDataLen)
	{
		int n = (((nDataLen & 3) == 0) ? ((unsigned int)nDataLen >> 2)
			: (((unsigned int)nDataLen >> 2) + 1));
		int i = 0;

		if (includeLength) {
			*ppresult = (int*)malloc(sizeof(int) * (n + 1));
			memset(*ppresult, 0, sizeof(int) * (n + 1));
			(*ppresult)[n] = nDataLen;
			n += 1;
		} else {
			*ppresult = (int*)malloc(sizeof(int) * n);
			memset(*ppresult, 0, sizeof(int) * n);
		}
		for (i = 0; i < (int)nDataLen; i++) {
			(*ppresult)[(unsigned int)i >> 2] |= (0x000000ff & pData[i]) << ((i & 3) << 3);
		}
		return n;
	}
	return 0;
}

int XXTEA::toByteArray(unsigned char ** ppresult, int * pData, int nDataLen, int includeLength)
{
	if (ppresult && pData && nDataLen)
	{
		int n = nDataLen << 2;
		int i = 0;

		if (includeLength) {
			int m = pData[nDataLen - 1];

			if (m > n) {
				return 0;
			} else {
				n = m;
			}
		}
		*ppresult = (unsigned char*)malloc(sizeof(unsigned char) * n);

		for (i = 0; i < n; i++) {
			(*ppresult)[i] = (unsigned char) ((pData[i >> 2] >> ((i & 3) << 3)) & 0xff);
		}
		return n;
	}
	return 0;
}

#define MX ((unsigned int)z >> 5 ^ y << 2) + ((unsigned int)y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z)

int * XXTEA::raw_encrypt(int * v, int vlen, int * k, int klen)
{
	int n = (int)vlen - 1;
	if (n < 1)
	{
		return v;
	}
	else
	{
		int * key = NULL;
		if (klen < 4)
		{
			key = (int*)malloc(sizeof(int) * 4);
			memset(key, 0, sizeof(int) * 4);
			memcpy(key, k, sizeof(int) * klen);
			k = key;
		}

		{
			int z = v[n], y = v[0], delta = 0x9E3779B9, sum = 0, e;
			int p, q = 6 + 52 / (n + 1);
			while (q-- > 0) {
				sum += delta;
				e = (unsigned int)sum >> 2 & 3;
				for (p = 0; p < n; p++) {
					y = v[p + 1];
					z = (v[p] += MX);
				}
				y = v[0];
				z = (v[n] += MX);
			}
		}
		if (key)
			free(key);
	}

	return v;
}

int * XXTEA::raw_decrypt(int * v, int vlen, int * k, int klen)
{
	int n = (int)vlen - 1;
	if (n < 1)
	{
		return v;
	}
	else
	{
		int * key = NULL;
		if (klen < 4) {
			key = (int*)malloc(sizeof(int) * 4);
			memset(key, 0, sizeof(int) * 4);
			memcpy(key, k, sizeof(int) * klen);
			k = key;
		}
		{
			//delta = 0x9E3779B9
			int z = v[n], y = v[0], delta = 0x9E3779B9, sum, e;
			int p, q = 6 + 52 / (n + 1);
			sum = q * delta;
			while (sum != 0) {
				e = (unsigned int)sum >> 2 & 3;
				for (p = n; p > 0; p--) {
					z = v[p - 1];
					y = (v[p] -= MX);
				}
				z = v[n];
				y = (v[0] -= MX);
				sum -= delta;
			}
		}

		if (key)
			free(key);
	}

	return v;
}

int XXTEA::xxtea_encrypt(unsigned char ** ppresult, const unsigned char* pData, int nDataLen, const unsigned char* pKey, int nKeyLen)
{
	if (!pData || nKeyLen == 0)
	{
		return 0;
	}	
	else
	{
		int * data = NULL;
		int * key = NULL;

		int datalen = toIntArray(&data,pData,nDataLen,1);
		int keylen = toIntArray(&key,pKey,nKeyLen,0);
		int retlen = 0;

		if (data && key)
		{
			retlen = toByteArray(ppresult, raw_encrypt(data, datalen, key, keylen), datalen, 0);
		}
		if (data)
			free(data);
		if (key)
			free(key);
		return retlen;
	}
	return 0;
}

int XXTEA::xxtea_decrypt(unsigned char ** ppresult, const unsigned char* pData, int nDataLen, const unsigned char* pKey, int nKeyLen)
{
	if (!pData || nKeyLen == 0)
	{
		return 0;

	}
	else
	{
		int * data = NULL;
		int * key = NULL;

		int datalen = toIntArray(&data,pData,nDataLen,0);
		int keylen = toIntArray(&key,pKey,nKeyLen,0);
		int retlen = 0;

		if (data && key)
		{
			retlen = toByteArray(ppresult, raw_decrypt(data, datalen, key, keylen), datalen, 1);
		}
		if (data)
			free(data);
		if (key)
			free(key);
		return retlen;
	}

	return 0;
}





const char * const g_szKey = "abcdefg";


char * XXTEA::hex2hexstr(unsigned char * pSrc, int nSrcLen)
{
	char * pDes = NULL;
	char * p;

	if (pSrc && nSrcLen > 0)
	{
		pDes = new char[(nSrcLen << 1) + 1];
		p = pDes;
		for (int i=0; i<nSrcLen; i++)
		{
			sprintf(p,"%02x",(unsigned char)pSrc[i]);
			p += 2;
		}
		*p = 0;
	}
	return pDes;
}


char XXTEA::intToHexChar(int x) 
{
	static const char HEX[16] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	return HEX[x];
}


string XXTEA::bytesToHexString(const unsigned char *in, size_t size) 
{
	string str;
	int t;
	int a;
	int b;

	for (size_t i = 0; i < size; ++i) 
	{
		t = in[i];
		a = t / 16;
		b = t % 16;
		str.append(1, intToHexChar(a));
		str.append(1, intToHexChar(b));
	}
		return str;
}

char *XXTEA::hexstr2hex(unsigned char * pSrc,int nSrcLen)
{
	char * pDes = NULL;
	char *a=new char[(nSrcLen>>1)+1];
	for (int i=0;i<nSrcLen;i++)
	{
		if (pSrc[i]>='a'&&pSrc[i]<='f')
		{
			pSrc[i]=pSrc[i]-97+10;
		}
		else if(pSrc[i]>='A'&&pSrc[i]<='F')
		{
			pSrc[i]=pSrc[i]-65+10;
		}
		else
		{
			pSrc[i]=pSrc[i]-48;
		}
	}
	int k=0;
	for (int j=0;j<nSrcLen;j++)
	{
		if (j%2!=0)
		{
			a[k++]=pSrc[j-1]*16+pSrc[j];
		}

	}
	a[k]='\0';
	return a;
}

//-----------------------------------------------------------------------
/*----------------------------------------

�������ƣ�int StrToHex(const char *str, char *hex, UINT nLength)

���ܣ����ַ�������һ������'\0'��β�ģ�ת��Ϊʮ�����Ʊ�ʾ���ַ���,�磺"HELLO"-->"48 45 4C 4C 4F"
nLength����ָ��Ϊ�ַ�����ʵ�ʳ���           
����˵����str--------��Ҫת�����ַ���
         hex -------ת������ַ���
        nLength----��Ҫת�����ַ����ĳ���(�Բ���'\0'�������޷������飬��Ҫָ������)
�� �� ֵ���㣬�ɹ�
----------------------------------------*/

int XXTEA::StrToHex(const char *str, char *hex , unsigned int nLength)
{
     int i;
     for (i=0;i<(int)nLength;i++)
     {
         sprintf(hex+i*2,"%.2X",str[i]&0xFF); //���ַ�ת��Ϊ��Ӧ��ʮ�����Ʊ�ʾ
     }
     return 0;
}

/*----------------------------------------
�������ƣ�int   HexToStr(const char *hex, char *str)
���ܣ���ʮ�������ַ���ת���ַ�����ʽ
�罫"F0140015AFAFAF0F" �����16���ַ����ַ���ת��Ϊ8���ֽڱ�ʾ���ֽڴ�
����˵����hex----ʮ�������ַ���
 str----ת������ַ���������ʵ����Ҫ��ת��ʱ00���ضϣ�
�� �� ֵ��0-------ת���ɹ�
        -1-------ת��ʧ��
----------------------------------------*/
int  XXTEA::HexToStr(const char *hex, char *str)

{
     int i,hlen,iData,iFlag,ilen;
     char ch;
     iData=0;
     iFlag=0;
     hlen=(int)strlen(hex);
	 ilen = 0;
     for (i=0;i<hlen;i++)
     {
         ch=hex[i];
         if ((ch>='0') && (ch<='9'))
         {
              ch-=0x30;
			  
         }
         else if ((ch>='a') && (ch<='f'))
         {
              ch-=0x57;
         }

         else if ((ch>='A') && (ch<='F'))
         {
              ch-=0x37;
         }
         else
         {
              *str='\0';
             return -1;
         }//end if
         if (0==iFlag)
         {
              iData=ch;
              iFlag=1;
         }
         else
         {
              iData=(iData<<4) +ch;
              iFlag=0;
              *str++=iData;
			  ilen++;
         }//enf if
     }//enf for
     *str='\0';
     return ilen;
}