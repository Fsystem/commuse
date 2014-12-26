#pragma once
//ÓÎÏ·½ÇÉ«ID
typedef short	GameCharID;
#define MAX_NAME_CREATE 12
#define MAX_NAME_STRING 32
#define MAX_PASSWORD_STRING 32
#define MAX_GUILD_NAME	16

#define _SET_FLAG( a, b ) ( a |= b )
#define _HAS_FLAG( a, b ) ( ( a & b ) == b )
#define _CLS_FLAG( a, b ) ( a &= (~b) )

//#define INVALID_CHAR_ID
