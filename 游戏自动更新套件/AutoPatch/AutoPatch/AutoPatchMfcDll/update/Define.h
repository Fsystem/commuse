#ifndef  __DEFINE_H__
#define  __DEFINE_H__

#define     MAX_BAR				100
typedef BOOL (CALLBACK* LOADCALL)(DWORD ,float,BOOL);
typedef void (CALLBACK* LOADSIZECALL)(DWORD);
typedef void (CALLBACK* LOADINFO)(char* , DWORD);


#endif