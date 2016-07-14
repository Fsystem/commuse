#ifndef __RegexOpt_H
#define __RegexOpt_H
class RegOpt
{
	typedef std::list<std::string> result_tyle;
	typedef const result_tyle& const_deff_result_tyle;
public:
	const_deff_result_tyle GetRegexResult(std::string sRegPattern,std::string sContent);
	static void RegexReplace(std::string& sContent,std::string sSearch,std::string sInstead);
protected:
private:
	result_tyle result;
};
#endif //__RegexOpt_H