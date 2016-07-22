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
	std::vector<std::ofstream*> fstreams;
	std::map<std::string,std::vector<std::string>> mapFileData;
	std::string msRetain;
};
#endif //__DelRepeat_H