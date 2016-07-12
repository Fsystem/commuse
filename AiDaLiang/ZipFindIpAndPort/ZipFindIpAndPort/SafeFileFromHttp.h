#ifndef __SafeFileFromHttp_H
#define __SafeFileFromHttp_H
class SaveFileFromHttp
{
public:
	void DownFromHttp(std::string sConfig,std::string sLocalDownDir);
	int GetTotal();
	int GetCurCnt();
	bool IsDownFinish();
protected:
	void DownThread(void* p);
private:
	

	//std::string sCurUrl;
	
};
#endif //__SafeFileFromHttp_H