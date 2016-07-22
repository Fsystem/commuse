#ifndef __DelRepeat_H
#define __DelRepeat_H


class DelRepeat
{
public:
	DelRepeat(const std::vector<std::string>& files,const std::string& sRetain);
	~DelRepeat();

	void Execute();
protected:
private:
	std::map<int,std::string> mapFiles;
	std::map<int,std::vector<std::string>> mapFileData;
	std::string msRetain;
};
#endif //__DelRepeat_H