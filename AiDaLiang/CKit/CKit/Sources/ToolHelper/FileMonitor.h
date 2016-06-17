#ifndef __FileMonitor_H
#define __FileMonitor_H

class FileMonMgr;

//-------------------------------------------------------------------------------
//文件监控回调通知处理钩子
struct IFileMonitorDelegate
{
	/** 此回调位多线程调用，请酌情进行线程同步
	*   @Author   : Double sword
	*   @dwAction : FILE_ACTION_ADDED ...
	*   @Return   :	无			
	*   @Date     :	2016-6-13
	*/
	virtual void OnFileChange(LPCSTR lpszFile,DWORD dwAction) = 0;
};

//-------------------------------------------------------------------------------
/** 文件监控
*   @ClassName	: 
*   @Purpose	:监控指定目录文件的变化（创建和大小变化）
*   @Author	: Double Sword
*   @Data	: 2016-6-7
*/
class FileMonitor
{
	friend class FileMonMgr;
protected:
	FileMonitor();
	~FileMonitor();

protected:
	BOOL StartMonitor(LPCSTR szDir,IFileMonitorDelegate* pSink = NULL);
	void StopMonitor();

	void FileMonitorThread();

private:
	HANDLE hThreadMonitor;
	HANDLE hDir;
	BOOL bRun;
	char chDir[MAX_PATH];
	IFileMonitorDelegate* mSink;
};

//-------------------------------------------------------------------------------
/** 文件监控管理类
*   @ClassName	: 
*   @Purpose	:监控除系统盘之外所有的盘文件变化监控服务
*   @Author	: Double Sword
*   @Data	: 2016-6-7
*/

class FileMonMgr : public SingletonBase<FileMonMgr>,public IFileMonitorDelegate
{
	friend class FileMonitor;
public:
	FileMonMgr();
	virtual ~FileMonMgr();

	BOOL StartService(BOOL bExeptRecycle,BOOL bExceptSysDrive = TRUE,IFileMonitorDelegate* pSink = NULL);
	void StopService();

	BOOL IsFileChanged();

	std::string GetSysDisk();
	std::vector<std::string> GetAllDisks();
	
protected:
	virtual void OnFileChange(LPCSTR lpszFile,DWORD dwAction);
private:
	IFileMonitorDelegate* mSink;
	FileMonitor** fileMonitors;
	int mDriveCnt;
	std::map<std::string,std::vector<DWORD>> mMapFileOpt;
};

#endif //__FileMonitor_H