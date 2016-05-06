/** 打印日志
*   @FileName  : CommLog.h
*   @Author    : Double Sword
*   @date      : 2011-8-28
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#ifndef __CommLog_H
#define __CommLog_H


//需要打印就定义不需要就不定义
//#define DEBUG_LOG   //--打印debug

//#define DEBUG_FILE //--打印文件

#define G_COM_PRINT_LOG COMMUSE::ComPrintLog::GetInstance()

#ifdef DEBUG_LOG
#	define LOGEVEN  G_COM_PRINT_LOG.LogEvenD
#elif defined(DEBUG_FILE)
#	define LOGEVEN  G_COM_PRINT_LOG.LogEvenF
#else
#	define LOGEVEN	G_COM_PRINT_LOG.LogEvenE
#endif //#ifdef DEBUG_LOG 

namespace COMMUSE
{
	/** 
	*   @ClassName	: ComPrintLog
	*   @Purpose	: 管理日志
	*
	*
	*
	*
	*   @Author	: Double Sword
	*   @Data	: 2011-8-28
	*/
	class ComPrintLog
	{
	public:
		ComPrintLog();
		~ComPrintLog();
		//静态实例
		static ComPrintLog& GetInstance()
		{
			static ComPrintLog c;
			return c;
		}

		//ini控制打印
		void InitLogState();
		int GetLogState(){return _nLogState;}

		//用debugview打印
#	ifdef UNICODE
		void LogEvenD (wchar_t* format,...);
#	else
		void LogEvenD (char* format,...);
#	endif
		
		//用File打印
#	ifdef UNICODE
		void LogEvenF (wchar_t* format,...);
#	else
		void LogEvenF (char* format,...);
#	endif

		//ini控制打印
#	ifdef UNICODE
		void LogEvenE (wchar_t* format,...);
#	else
		void LogEvenE (char* format,...);
#	endif

	protected:
	private:
		FILE *				_LogFile ;					//打印文件句柄
		char				_szLogFilePath[MAX_PATH];	//log文件路径
		char				_szAppFileName[MAX_PATH];	//应用程序名
		int					_nLogState;					//是否打印日志
		char				_szLogStateIniPath[MAX_PATH];//ini文件路径

		CRITICAL_SECTION	_cs_log;				//打印文件锁
		//初始化
		void PrintInit();
		//反初始化
		void PrintUnInit();
	};
}


#endif //__CommLog_H




