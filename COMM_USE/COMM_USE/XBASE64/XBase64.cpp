#pragma once
/*#include "stdafx.h"*/
#include <stdio.h>
#include <string>
#include "XBase64.h"

#pragma warning(disable:4996)

namespace COMMUSE
{
	void XBase64::Encode(char *src, int src_len, char *dst)
{

	int i = 0, j = 0;

	char base64_map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



	for (; i < src_len - src_len % 3; i += 3) {

		dst[j++] = base64_map[(src[i] >> 2) & 0x3F];

		dst[j++] = base64_map[((src[i] << 4) & 0x30) + ((src[i + 1] >> 4) & 0xF)];

		dst[j++] = base64_map[((src[i + 1] << 2) & 0x3C) + ((src[i + 2] >> 6) & 0x3)];

		dst[j++] = base64_map[src[i + 2] & 0x3F];

	}



	if (src_len % 3 == 1) {

		dst[j++] = base64_map[(src[i] >> 2) & 0x3F];

		dst[j++] = base64_map[(src[i] << 4) & 0x30];

		dst[j++] = '=';

		dst[j++] = '=';

	}

	else if (src_len % 3 == 2) {

		dst[j++] = base64_map[(src[i] >> 2) & 0x3F];

		dst[j++] = base64_map[((src[i] << 4) & 0x30) + ((src[i + 1] >> 4) & 0xF)];

		dst[j++] = base64_map[(src[i + 1] << 2) & 0x3C];

		dst[j++] = '=';

	}



	dst[j] = '\0';

}



void XBase64::Decode(char *src, int src_len, char *dst)
{
	if (src && dst)
	{


		int i = 0, j = 0;

		unsigned char base64_decode_map[256] = {

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 62, 255, 255, 255, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255,

			255, 0, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,

			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255, 255, 26, 27, 28,

			29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,

			49, 50, 51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,

			255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};



			for (; i < src_len; i += 4) {

				dst[j++] = base64_decode_map[src[i]] << 2 |

					base64_decode_map[src[i + 1]] >> 4;

				dst[j++] = base64_decode_map[src[i + 1]] << 4 |

					base64_decode_map[src[i + 2]] >> 2;

				dst[j++] = base64_decode_map[src[i + 2]] << 6 |

					base64_decode_map[src[i + 3]];

			}



			dst[j] = '\0';

	}

}


//-----------------------------------------------------------------------
/*----------------------------------------

函数名称：int StrToHex(const char *str, char *hex, UINT nLength)

功能：将字符串（不一定是以'\0'结尾的）转化为十六进制表示的字符串,如："HELLO"-->"48 45 4C 4C 4F"
nLength可以指定为字符串的实际长度           
参数说明：str--------需要转换的字符串
         hex -------转换后的字符串
        nLength----需要转换的字符串的长度(对不以'\0'结束的无符号数组，需要指明长度)
返 回 值：零，成功
----------------------------------------*/

int XBase64::StrToHex(const char *str, char *hex , unsigned int nLength)
{
     int i;
     for (i=0;i<(int)nLength;i++)
     {
         sprintf(hex+i*2,"%02X",str[i]&0xFF); //将字符转化为相应的十六进制表示
     }
     return 0;
}

/*----------------------------------------
函数名称：int   HexToStr(const char *hex, char *str)
功能：将十六进制字符串转换字符串形式
如将"F0140015AFAFAF0F" 这个有16个字符的字符串转换为8个字节表示的字节串
参数说明：hex----十六进制字符串
 str----转换后的字符串（根据实际需要，转换时00不截断）
返 回 值：0-------转换成功
        -1-------转换失败
----------------------------------------*/
int  XBase64::HexToStr(const char *hex, char *str)

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
}

