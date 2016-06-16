 

#ifndef __MYYRC4_H__
#define __MYYRC4_H__
//dfasgkpowgj9%@#^Tfdg9yulkj325t32twe
/*加密解密都用这个函数，加密或解密后长度保持不变*/
/*注意：keylen不能>=256，srclen不能<=0*/
void yrc4 (const unsigned char *key, int keylen, const unsigned char *src, int srclen, unsigned char *dst);

#endif /* __YRC4_H__ */
