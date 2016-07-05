#include "StdAfx.h"
#include "Encrypt.h"

//数据大小
#define ENCRYPT_KEY_LEN					5									//密钥长度
#define CountArray(arr)					(sizeof(arr)/sizeof(arr[0]))

//////////////////////////////////////////////////////////////////////////

#define S11		7
#define S12		12
#define S13		17
#define S14		22
#define S21		5
#define S22		9
#define S23 	14
#define S24 	20
#define S31 	4
#define S32 	11
#define S33 	16
#define S34 	23
#define S41 	6
#define S42 	10
#define S43 	15
#define S44 	21

#define F(x,y,z) (((x)&(y))|((~x)&(z)))
#define G(x,y,z) (((x)&(z))|((y)&(~z)))
#define H(x,y,z) ((x)^(y)^(z))
#define I(x,y,z) ((y)^((x)|(~z)))

#define ROTATE_LEFT(x,n) (((x)<<(n))|((x)>>(32-(n))))

#define FF(a,b,c,d,x,s,ac)													\
{																			\
	(a)+=F((b),(c),(d))+(x)+(unsigned long int)(ac);						\
	(a)=ROTATE_LEFT((a),(s));												\
	(a)+=(b);																\
}

#define GG(a,b,c,d,x,s,ac)													\
{																			\
	(a)+=G((b),(c),(d))+(x)+(unsigned long int)(ac);						\
	(a)=ROTATE_LEFT ((a),(s));												\
	(a)+=(b);																\
}

#define HH(a,b,c,d,x,s,ac)													\
{																			\
	(a)+=H((b),(c),(d))+(x)+(unsigned long int)(ac);						\
	(a)=ROTATE_LEFT((a),(s));												\
	(a)+=(b);																\
}

#define II(a,b,c,d,x,s,ac)													\
{																			\
	(a)+=I((b),(c),(d))+(x)+(unsigned long int)(ac);						\
	(a)=ROTATE_LEFT((a),(s));												\
	(a)+=(b);																\
}

//////////////////////////////////////////////////////////////////////////

//MD5 加密类
class CMD5
{
	//变量定义
private:
	unsigned long int				state[4];
	unsigned long int				count[2];
	unsigned char					buffer[64];
	unsigned char					PADDING[64];

	//函数定义
public:
	//构造函数
	CMD5() { MD5Init(); }

	//功能函数
public:
	//最终结果
	void MD5Final(unsigned char digest[16]);
	//设置数值
	void MD5Update(unsigned char * input, unsigned int inputLen);

	//内部函数
private:
	//初始化
	void MD5Init();
	//置位函数
	void MD5_memset(unsigned char * output, int value, unsigned int len);
	//拷贝函数
	void MD5_memcpy(unsigned char * output, unsigned char * input, unsigned int len);
	//转换函数
	void MD5Transform(unsigned long int state[4], unsigned char block[64]);
	//编码函数
	void Encode(unsigned char * output, unsigned long int * input, unsigned int len);
	//解码函数
	void Decode(unsigned long int *output, unsigned char * input, unsigned int len);
};

//////////////////////////////////////////////////////////////////////////

//初始化
void CMD5::MD5Init()
{
	count[0]=0;
	count[1]=0;
	state[0]=0x67452301;
	state[1]=0xefcdab89;
	state[2]=0x98badcfe;
	state[3]=0x10325476;
	MD5_memset(PADDING,0,sizeof(PADDING));
	*PADDING=0x80;
	return;
}

//更新函数
void CMD5::MD5Update (unsigned char * input, unsigned int inputLen)
{
	unsigned int i,index,partLen;
	index=(unsigned int)((this->count[0]>>3)&0x3F);
	if ((count[0]+=((unsigned long int)inputLen<<3))<((unsigned long int)inputLen<<3)) count[1]++;
	count[1]+=((unsigned long int)inputLen>>29);
	partLen=64-index;
	if (inputLen>=partLen) 
	{
		MD5_memcpy((unsigned char*)&buffer[index],(unsigned char *)input,partLen);
		MD5Transform(state,buffer);
		for (i=partLen;i+63<inputLen;i+=64) MD5Transform(state,&input[i]);
		index=0;
	}
	else i=0;
	MD5_memcpy((unsigned char*)&buffer[index],(unsigned char *)&input[i],inputLen-i);
	return;
}

//最终结果
void CMD5::MD5Final(unsigned char digest[16])
{
	unsigned char bits[8];
	unsigned int index,padLen;
	Encode(bits,count,8);
	index=(unsigned int)((count[0]>>3)&0x3f);
	padLen=(index<56)?(56-index):(120-index);
	MD5Update( PADDING,padLen);
	MD5Update(bits,8);
	Encode(digest,state,16);
	MD5_memset((unsigned char*)this,0,sizeof (*this));
	MD5Init();
	return;
}

//转换函数
void CMD5::MD5Transform(unsigned long int state[4], unsigned char block[64])
{
	unsigned long int a=state[0],b=state[1],c=state[2],d=state[3],x[16];
	Decode(x,block,64);

	FF(a,b,c,d,x[ 0],S11,0xd76aa478); /* 1 */
	FF(d,a,b,c,x[ 1],S12,0xe8c7b756); /* 2 */
	FF(c,d,a,b,x[ 2],S13,0x242070db); /* 3 */
	FF(b,c,d,a,x[ 3],S14,0xc1bdceee); /* 4 */
	FF(a,b,c,d,x[ 4],S11,0xf57c0faf); /* 5 */
	FF(d,a,b,c,x[ 5],S12,0x4787c62a); /* 6 */
	FF(c,d,a,b,x[ 6],S13,0xa8304613); /* 7 */
	FF(b,c,d,a,x[ 7],S14,0xfd469501); /* 8 */
	FF(a,b,c,d,x[ 8],S11,0x698098d8); /* 9 */
	FF(d,a,b,c,x[ 9],S12,0x8b44f7af); /* 10 */
	FF(c,d,a,b,x[10],S13,0xffff5bb1); /* 11 */
	FF(b,c,d,a,x[11],S14,0x895cd7be); /* 12 */
	FF(a,b,c,d,x[12],S11,0x6b901122); /* 13 */
	FF(d,a,b,c,x[13],S12,0xfd987193); /* 14 */
	FF(c,d,a,b,x[14],S13,0xa679438e); /* 15 */
	FF(b,c,d,a,x[15],S14,0x49b40821); /* 16 */

	GG(a,b,c,d,x[ 1],S21,0xf61e2562); /* 17 */
	GG(d,a,b,c,x[ 6],S22,0xc040b340); /* 18 */
	GG(c,d,a,b,x[11],S23,0x265e5a51); /* 19 */
	GG(b,c,d,a,x[ 0],S24,0xe9b6c7aa); /* 20 */
	GG(a,b,c,d,x[ 5],S21,0xd62f105d); /* 21 */
	GG(d,a,b,c,x[10],S22,0x2441453);  /* 22 */
	GG(c,d,a,b,x[15],S23,0xd8a1e681); /* 23 */
	GG(b,c,d,a,x[ 4],S24,0xe7d3fbc8); /* 24 */
	GG(a,b,c,d,x[ 9],S21,0x21e1cde6); /* 25 */
	GG(d,a,b,c,x[14],S22,0xc33707d6); /* 26 */
	GG(c,d,a,b,x[ 3],S23,0xf4d50d87); /* 27 */
	GG(b,c,d,a,x[ 8],S24,0x455a14ed); /* 28 */
	GG(a,b,c,d,x[13],S21,0xa9e3e905); /* 29 */
	GG(d,a,b,c,x[ 2],S22,0xfcefa3f8); /* 30 */
	GG(c,d,a,b,x[ 7],S23,0x676f02d9); /* 31 */
	GG(b,c,d,a,x[12],S24,0x8d2a4c8a); /* 32 */

	HH(a,b,c,d,x[ 5],S31,0xfffa3942); /* 33 */
	HH(d,a,b,c,x[ 8],S32,0x8771f681); /* 34 */
	HH(c,d,a,b,x[11],S33,0x6d9d6122); /* 35 */
	HH(b,c,d,a,x[14],S34,0xfde5380c); /* 36 */
	HH(a,b,c,d,x[ 1],S31,0xa4beea44); /* 37 */
	HH(d,a,b,c,x[ 4],S32,0x4bdecfa9); /* 38 */
	HH(c,d,a,b,x[ 7],S33,0xf6bb4b60); /* 39 */
	HH(b,c,d,a,x[10],S34,0xbebfbc70); /* 40 */
	HH(a,b,c,d,x[13],S31,0x289b7ec6); /* 41 */
	HH(d,a,b,c,x[ 0],S32,0xeaa127fa); /* 42 */
	HH(c,d,a,b,x[ 3],S33,0xd4ef3085); /* 43 */
	HH(b,c,d,a,x[ 6],S34,0x4881d05);  /* 44 */
	HH(a,b,c,d,x[ 9],S31,0xd9d4d039); /* 45 */
	HH(d,a,b,c,x[12],S32,0xe6db99e5); /* 46 */
	HH(c,d,a,b,x[15],S33,0x1fa27cf8); /* 47 */
	HH(b,c,d,a,x[ 2],S34,0xc4ac5665); /* 48 */

	II(a,b,c,d,x[ 0],S41,0xf4292244); /* 49 */
	II(d,a,b,c,x[ 7],S42,0x432aff97); /* 50 */
	II(c,d,a,b,x[14],S43,0xab9423a7); /* 51 */
	II(b,c,d,a,x[ 5],S44,0xfc93a039); /* 52 */
	II(a,b,c,d,x[12],S41,0x655b59c3); /* 53 */
	II(d,a,b,c,x[ 3],S42,0x8f0ccc92); /* 54 */
	II(c,d,a,b,x[10],S43,0xffeff47d); /* 55 */
	II(b,c,d,a,x[ 1],S44,0x85845dd1); /* 56 */
	II(a,b,c,d,x[ 8],S41,0x6fa87e4f); /* 57 */
	II(d,a,b,c,x[15],S42,0xfe2ce6e0); /* 58 */
	II(c,d,a,b,x[ 6],S43,0xa3014314); /* 59 */
	II(b,c,d,a,x[13],S44,0x4e0811a1); /* 60 */
	II(a,b,c,d,x[ 4],S41,0xf7537e82); /* 61 */
	II(d,a,b,c,x[11],S42,0xbd3af235); /* 62 */
	II(c,d,a,b,x[ 2],S43,0x2ad7d2bb); /* 63 */
	II(b,c,d,a,x[ 9],S44,0xeb86d391); /* 64 */

	state[0]+=a;
	state[1]+=b;
	state[2]+=c;
	state[3]+=d;

	MD5_memset((unsigned char *)x,0,sizeof(x));

	return;
}

//编码函数
void CMD5::Encode(unsigned char * output, unsigned long int * input,unsigned int len)
{
	unsigned int i, j;
	for (i=0,j=0;j<len;i++,j+=4)
	{
		output[j]=(unsigned char)(input[i]&0xff);
		output[j+1]=(unsigned char)((input[i]>>8)&0xff);
		output[j+2]=(unsigned char)((input[i]>>16)&0xff);
		output[j+3]=(unsigned char)((input[i]>>24)&0xff);
	}
	return;
}

//解码函数
void CMD5::Decode(unsigned long int *output, unsigned char *input, unsigned int len)
{
	unsigned int i,j;
	for (i=0,j=0;j<len;i++,j+=4)
	{
		output[i]=((unsigned long int)input[j])|(((unsigned long int)input[j+1])<<8)|
			(((unsigned long int)input[j+2])<<16)|(((unsigned long int)input[j+3])<< 24);
	}
	return;
}

//拷贝函数
void CMD5::MD5_memcpy(unsigned char * output, unsigned char * input,unsigned int len)
{
	for (unsigned int i=0;i<len;i++) output[i]=input[i];
}

//置位函数
void CMD5::MD5_memset (unsigned char * output, int value, unsigned int len)
{
	for (unsigned int i=0;i<len;i++) ((char *)output)[i]=(char)value;
}

//////////////////////////////////////////////////////////////////////////

//生成密文
void CMD5Encrypt::EncryptData(LPCTSTR pszSrcData, TCHAR szMD5Result[33])
{
	//加密密文
	CMD5 MD5Encrypt;
	UCHAR szResult[16];
	MD5Encrypt.MD5Update((unsigned char *)pszSrcData,lstrlen(pszSrcData)*sizeof(TCHAR));
	MD5Encrypt.MD5Final(szResult);

	//输出结果
	szMD5Result[0]=0;
	for (int i=0;i<16;i++) sprintf(&szMD5Result[i*2],"%02x",szResult[i]);

	return;
}

//////////////////////////////////////////////////////////////////////////

//生成密文
WORD CXOREncrypt::EncryptData(LPCTSTR pszSrcData, LPTSTR pszEncrypData, WORD wSize)
{
	//效验参数
	ASSERT(wSize>0);
	ASSERT(pszSrcData!=NULL);
	ASSERT(pszEncrypData!=NULL);

	//初始化参数
	pszEncrypData[0]=0;

	//生成密钥
	unsigned char szRandKey[ENCRYPT_KEY_LEN];
	szRandKey[0]=lstrlen(pszSrcData);
	for (WORD i=1;i<CountArray(szRandKey);i++) szRandKey[i]=rand()%255;

	//生成密文
	BYTE bTempCode=0;
	WORD wTimes=(szRandKey[0]+ENCRYPT_KEY_LEN-1)/ENCRYPT_KEY_LEN*ENCRYPT_KEY_LEN;
	if ((wTimes*4+1)>wSize) return wTimes*4+1;
	for (WORD i=0;i<wTimes;i++)
	{
		if (i<szRandKey[0]) bTempCode=pszSrcData[i]^szRandKey[i%ENCRYPT_KEY_LEN];
		else bTempCode=szRandKey[i%ENCRYPT_KEY_LEN]^(rand()%255);
		_snprintf(pszEncrypData+i*4,5,TEXT("%02x%02x"),szRandKey[i%ENCRYPT_KEY_LEN],bTempCode);
	}

	return wTimes*4;
}

//解开密文
WORD CXOREncrypt::CrevasseData(LPCTSTR pszEncrypData, LPTSTR pszSrcData, WORD wSize)
{
	//效验参数
	ASSERT(wSize>0);
	ASSERT(pszSrcData!=NULL);
	ASSERT(pszEncrypData!=NULL);

	//初始化参数
	pszSrcData[0]=0;

	//效验长度
	WORD wEncrypPassLen=lstrlen(pszEncrypData);
	if (wEncrypPassLen<ENCRYPT_KEY_LEN*4) return 0;

	//效验长度
	TCHAR * pEnd=NULL;
	TCHAR szTempBuffer[3]={pszEncrypData[0],pszEncrypData[1],0};
	WORD wSoureLength=(WORD)strtol(szTempBuffer,&pEnd,16);
	if (wEncrypPassLen!=((wSoureLength+ENCRYPT_KEY_LEN-1)/ENCRYPT_KEY_LEN*ENCRYPT_KEY_LEN*4)) return 0;
	if (wSoureLength>(wSize-1)) return wSoureLength;

	//解开密码
	unsigned char szKeyBuffer[3];
	for (int i=0;i<wSoureLength;i++)
	{
		szKeyBuffer[0]=pszEncrypData[i*4];
		szKeyBuffer[1]=pszEncrypData[i*4+1];
		szKeyBuffer[2]=0;
		szTempBuffer[0]=pszEncrypData[i*4+2];
		szTempBuffer[1]=pszEncrypData[i*4+3];
		szTempBuffer[2]=0;
		UINT uKey=strtol((LPCTSTR)szKeyBuffer,&pEnd,16);
		UINT uEncrypt=strtol(szTempBuffer,&pEnd,16);
		pszSrcData[i]=(TCHAR)((BYTE)uKey^(BYTE)uEncrypt);
	}
	pszSrcData[wSoureLength]=0;

	return wSoureLength;
}

//////////////////////////////////////////////////////////////////////////

#define PVERSION41_CHAR   '*'
#define SHA1_HASH_SIZE 20 /* Hash size in bytes */
#define SHA1CircularShift(bits,word)	(((word) << (bits)) | ((word) >> (32-(bits)))) /*  Define the SHA1 circular left shift macro */
#define bzero(x,y)      memset((x),'\0',(y))

typedef signed char int8;       /* Signed integer >= 8  bits */
typedef struct SHA1_CONTEXT
{
	DWORDLONG  Length;            /* Message length in bits      */
	DWORD Intermediate_Hash[SHA1_HASH_SIZE/4]; /* Message Digest  */
	int Computed;                 /* Is the digest computed?         */
	int Corrupted;                /* Is the message digest corrupted? */
	short Message_Block_Index;    /* Index into message block array   */
	byte Message_Block[64];      /* 512-bit message blocks      */
} SHA1_CONTEXT;

char NEAR _dig_vec_upper[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char NEAR _dig_vec_lower[] ="0123456789abcdefghijklmnopqrstuvwxyz";

const DWORD sha_const_key[5]=
{
  0x67452301,
  0xEFCDAB89,
  0x98BADCFE,
  0x10325476,
  0xC3D2E1F0
};

enum sha_result_codes
{
  SHA_SUCCESS = 0,
  SHA_NULL,             /* Null pointer parameter */
  SHA_INPUT_TOO_LONG,   /* input data too long */
  SHA_STATE_ERROR       /* called Input after Result */
};

/*
  Process the next 512 bits of the message stored in the Message_Block array.

  SYNOPSIS
    SHA1ProcessMessageBlock()

   DESCRIPTION
     Many of the variable names in this code, especially the single
     character names, were used because those were the names used in
     the publication.
*/

/* Constants defined in SHA-1	*/
static const DWORD  K[]=
{
  0x5A827999,
  0x6ED9EBA1,
  0x8F1BBCDC,
  0xCA62C1D6
};

static void SHA1ProcessMessageBlock(SHA1_CONTEXT *context)
{
  int		t;		   /* Loop counter		  */
  DWORD	temp;		   /* Temporary word value	  */
  DWORD	W[80];		   /* Word sequence		  */
  DWORD	A, B, C, D, E;	   /* Word buffers		  */
  int idx;

  /*
    Initialize the first 16 words in the array W
  */

  for (t = 0; t < 16; t++)
  {
    idx=t*4;
    W[t] = context->Message_Block[idx] << 24;
    W[t] |= context->Message_Block[idx + 1] << 16;
    W[t] |= context->Message_Block[idx + 2] << 8;
    W[t] |= context->Message_Block[idx + 3];
  }


  for (t = 16; t < 80; t++)
  {
    W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
  }

  A = context->Intermediate_Hash[0];
  B = context->Intermediate_Hash[1];
  C = context->Intermediate_Hash[2];
  D = context->Intermediate_Hash[3];
  E = context->Intermediate_Hash[4];

  for (t = 0; t < 20; t++)
  {
    temp= SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
    E = D;
    D = C;
    C = SHA1CircularShift(30,B);
    B = A;
    A = temp;
  }

  for (t = 20; t < 40; t++)
  {
    temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
    E = D;
    D = C;
    C = SHA1CircularShift(30,B);
    B = A;
    A = temp;
  }

  for (t = 40; t < 60; t++)
  {
    temp= (SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] +
	   K[2]);
    E = D;
    D = C;
    C = SHA1CircularShift(30,B);
    B = A;
    A = temp;
  }

  for (t = 60; t < 80; t++)
  {
    temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
    E = D;
    D = C;
    C = SHA1CircularShift(30,B);
    B = A;
    A = temp;
  }

  context->Intermediate_Hash[0] += A;
  context->Intermediate_Hash[1] += B;
  context->Intermediate_Hash[2] += C;
  context->Intermediate_Hash[3] += D;
  context->Intermediate_Hash[4] += E;

  context->Message_Block_Index = 0;
}


/*
  Pad message

  SYNOPSIS
    SHA1PadMessage()
    context: [in/out]		The context to pad

  DESCRIPTION
    According to the standard, the message must be padded to an even
    512 bits.  The first padding bit must be a '1'. The last 64 bits
    represent the length of the original message.  All bits in between
    should be 0.  This function will pad the message according to
    those rules by filling the Message_Block array accordingly.  It
    will also call the ProcessMessageBlock function provided
    appropriately. When it returns, it can be assumed that the message
    digest has been computed.

*/

static void SHA1PadMessage(SHA1_CONTEXT *context)
{
  /*
    Check to see if the current message block is too small to hold
    the initial padding bits and length.  If so, we will pad the
    block, process it, and then continue padding into a second
    block.
  */

  int i=context->Message_Block_Index;

  if (i > 55)
  {
    context->Message_Block[i++] = 0x80;
    bzero((char*) &context->Message_Block[i],
	  sizeof(context->Message_Block[0])*(64-i));
    context->Message_Block_Index=64;

    /* This function sets context->Message_Block_Index to zero	*/
    SHA1ProcessMessageBlock(context);

    bzero((char*) &context->Message_Block[0],
	  sizeof(context->Message_Block[0])*56);
    context->Message_Block_Index=56;
  }
  else
  {
    context->Message_Block[i++] = 0x80;
    bzero((char*) &context->Message_Block[i],
	  sizeof(context->Message_Block[0])*(56-i));
    context->Message_Block_Index=56;
  }
  
  /*
    Store the message length as the last 8 octets
  */

  context->Message_Block[56] = (int8) (context->Length >> 56);
  context->Message_Block[57] = (int8) (context->Length >> 48);
  context->Message_Block[58] = (int8) (context->Length >> 40);
  context->Message_Block[59] = (int8) (context->Length >> 32);
  context->Message_Block[60] = (int8) (context->Length >> 24);
  context->Message_Block[61] = (int8) (context->Length >> 16);
  context->Message_Block[62] = (int8) (context->Length >> 8);
  context->Message_Block[63] = (int8) (context->Length);

  SHA1ProcessMessageBlock(context);
}

int mysql_sha1_reset(SHA1_CONTEXT *context)
{
#ifndef DBUG_OFF
	if (!context)
		return SHA_NULL;
#endif

	context->Length		  = 0;
	context->Message_Block_Index	  = 0;

	context->Intermediate_Hash[0]   = sha_const_key[0];
	context->Intermediate_Hash[1]   = sha_const_key[1];
	context->Intermediate_Hash[2]   = sha_const_key[2];
	context->Intermediate_Hash[3]   = sha_const_key[3];
	context->Intermediate_Hash[4]   = sha_const_key[4];

	context->Computed   = 0;
	context->Corrupted  = 0;

	return SHA_SUCCESS;
}

/*
   Return the 160-bit message digest into the array provided by the caller

  SYNOPSIS
    mysql_sha1_result()
    context [in/out]		The context to use to calculate the SHA-1 hash.
    Message_Digest: [out]	Where the digest is returned.

  DESCRIPTION
    NOTE: The first octet of hash is stored in the 0th element,
	  the last octet of hash in the 19th element.

 RETURN
   SHA_SUCCESS		ok
   != SHA_SUCCESS	sha Error Code.
*/

int mysql_sha1_result(SHA1_CONTEXT *context,byte Message_Digest[SHA1_HASH_SIZE])
{
  int i;

#ifndef DBUG_OFF
  if (!context || !Message_Digest)
    return SHA_NULL;

  if (context->Corrupted)
    return context->Corrupted;
#endif

  if (!context->Computed)
  {
    SHA1PadMessage(context);
     /* message may be sensitive, clear it out */
    bzero((char*) context->Message_Block,64);
    context->Length   = 0;    /* and clear length  */
    context->Computed = 1;
  }

  for (i = 0; i < SHA1_HASH_SIZE; i++)
    Message_Digest[i] = (byte)((context->Intermediate_Hash[i>>2] >> 8* ( 3 - ( i & 0x03 ) )));
  return SHA_SUCCESS;
}

/*
  Accepts an array of octets as the next portion of the message.

  SYNOPSIS
   mysql_sha1_input()
   context [in/out]	The SHA context to update
   message_array	An array of characters representing the next portion
			of the message.
  length		The length of the message in message_array

 RETURN
   SHA_SUCCESS		ok
   != SHA_SUCCESS	sha Error Code.
*/

int mysql_sha1_input(SHA1_CONTEXT *context, const byte *message_array,unsigned length)
{
  if (!length)
    return SHA_SUCCESS;

#ifndef DBUG_OFF
  /* We assume client konows what it is doing in non-debug mode */
  if (!context || !message_array)
    return SHA_NULL;
  if (context->Computed)
    return (context->Corrupted= SHA_STATE_ERROR);
  if (context->Corrupted)
    return context->Corrupted;
#endif

  while (length--)
  {
    context->Message_Block[context->Message_Block_Index++]=
      (*message_array & 0xFF);
    context->Length  += 8;  /* Length is in bits */

#ifndef DBUG_OFF
    /*
      Then we're not debugging we assume we never will get message longer
      2^64 bits.
    */
    if (context->Length == 0)
      return (context->Corrupted= 1);	   /* Message is too long */
#endif

    if (context->Message_Block_Index == 64)
    {
      SHA1ProcessMessageBlock(context);
    }
    message_array++;
  }
  return SHA_SUCCESS;
}

char *octet2hex(char *to, const char *str, DWORD len)
{
	const char *str_end= str + len; 
	for (; str != str_end; ++str)
	{
		*to++= _dig_vec_upper[((byte) *str) >> 4];
		*to++= _dig_vec_upper[((byte) *str) & 0x0F];
	}
	*to= '\0';
	return to;
}

void my_make_scrambled_password(char *to, const char *password,size_t pass_len)
{
	SHA1_CONTEXT sha1_context;
	byte hash_stage2[SHA1_HASH_SIZE];

	mysql_sha1_reset(&sha1_context);
	/* stage 1: hash password */
	mysql_sha1_input(&sha1_context, (byte *) password, (DWORD) pass_len);
	mysql_sha1_result(&sha1_context, (byte *) to);
	/* stage 2: hash stage1 output */
	mysql_sha1_reset(&sha1_context);
	mysql_sha1_input(&sha1_context, (byte *) to, SHA1_HASH_SIZE);
	/* separate buffer is used to pass 'to' in octet2hex */
	mysql_sha1_result(&sha1_context, hash_stage2);
	/* convert hash_stage2 to hex string */
	*to++= PVERSION41_CHAR;
	octet2hex(to, (const char*)hash_stage2, SHA1_HASH_SIZE);
}

void CMySqlPWEncrypt::EncryptData( LPCTSTR pszSrcData, TCHAR pszEncrypData[SCRAMBLED_PASSWORD_CHAR_LENGTH] )
{
	my_make_scrambled_password(pszEncrypData,pszSrcData,strlen(pszSrcData));
}
