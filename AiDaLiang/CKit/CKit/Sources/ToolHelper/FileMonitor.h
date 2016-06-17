#ifndef __FileMonitor_H
#define __FileMonitor_H

class FileMonMgr;

//-------------------------------------------------------------------------------
//�ļ���ػص�֪ͨ������
struct IFileMonitorDelegate
{
	/** �˻ص�λ���̵߳��ã�����������߳�ͬ��
	*   @Author   : Double sword
	*   @dwAction : FILE_ACTION_ADDED ...
	*   @Return   :	��			
	*   @Date     :	2016-6-13
	*/
	virtual void OnFileChange(LPCSTR lpszFile,DWORD dwAction) = 0;
};

//-------------------------------------------------------------------------------
/** �ļ����
*   @ClassName	: 
*   @Purpose	:���ָ��Ŀ¼�ļ��ı仯�������ʹ�С�仯��
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
/** �ļ���ع�����
*   @ClassName	: 
*   @Purpose	:��س�ϵͳ��֮�����е����ļ��仯��ط���
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