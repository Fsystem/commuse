#pragma once


namespace uxipack
{
	class MD5
	{
	public:
		MD5(void);
		~MD5(void);
		char* MD5String( char* string ); 

		bool MD5Check( char *md5string, char* string ); 

		/*char* MD5TowString( char *string ) ;*/
	};
}

