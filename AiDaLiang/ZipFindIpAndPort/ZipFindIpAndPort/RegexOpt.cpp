#include "stdafx.h"
#include "RegexOpt.h"

#include <boost/regex.hpp>

RegOpt::const_deff_result_tyle RegOpt::GetRegexResult(std::string sRegPattern,std::string sContent)
{
	result.clear();
	if (sRegPattern.empty()==false)
	{
		try
		{
			boost::regex rg(sRegPattern,boost::regex::icase|boost::regex::perl);
			boost::smatch m;

			/*boost::sregex_iterator it1(sContent.begin(),sContent.end(),m,rg);
			boost::sregex_iterator it2;
			while(it1!=it2)
			{
			result.push_back(it1->str());
			it1++;
			}*/

			std::string::const_iterator sit1 = sContent.begin();
			std::string::const_iterator sit2 = sContent.end();
			while(boost::regex_search(sit1,sit2,m,rg/*,boost::match_not_bol|boost::match_perl|boost::match_posix*/))
			{
				result.push_back( m[0]);
				sit1 = m[1].second;
			}
		}
		catch(const boost::bad_expression & e)
		{
			MessageBoxA(NULL,e.what(),NULL,MB_OK);
		}
		
	}
	
	return result;
}