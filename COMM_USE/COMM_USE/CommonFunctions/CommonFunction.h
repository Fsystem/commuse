
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>   
#include <functional> 
#include <windows.h>
#include <sstream>

using namespace std;

namespace COMMUSE
{
	inline string&  lTrim(string   &ss)   
	{   
		string::iterator   p=find_if(ss.begin(),ss.end(),not1(ptr_fun(isspace)));   
		ss.erase(ss.begin(),p);   
		return  ss;   
	}   

	inline  string&  rTrim(string   &ss)   
	{   
		string::reverse_iterator  p=find_if(ss.rbegin(),ss.rend(),not1(ptr_fun(isspace)));   
		ss.erase(p.base(),ss.end());   
		return   ss;   
	}   

	inline string&   trim(string   &st)   
	{   
		lTrim(rTrim(st));   
		return   st;   
	}   


	inline string getSubofNo(const string& str,const string& substr) 
	{ 
		//return str.substr(str.find_first_not_of(">")); 
		return str.substr(0,str.find_last_not_of(substr)+1); 

	}  

	inline void DbgOutInt(string label, int value ) {
		stringstream strs;
		strs << value;
		label.append(strs.str()) ;
		const char *c_str =label.c_str() ;
		OutputDebugStringA( c_str ) ;
	}
}