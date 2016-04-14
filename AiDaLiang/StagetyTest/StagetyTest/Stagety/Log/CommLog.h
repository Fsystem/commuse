/** ��ӡ��־
*   @FileName  : CommLog.h
*   @Author    : Double Sword
*   @date      : 2011-8-28
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : --
*/
#ifndef __CommLog_H
#define __CommLog_H


//��Ҫ��ӡ�Ͷ��岻��Ҫ�Ͳ�����
//#define DEBUG_LOG   //--��ӡdebug

#define DEBUG_FILE //--��ӡ�ļ�

#define G_COM_PRINT_LOG COMMUSE::ComPrintLog::GetInstance()

#ifdef DEBUG_LOG
#	define LOGEVEN  G_COM_PRINT_LOG.LogEvenD
#elif defined(DEBUG_FILE)
#	define LOGEVEN  G_COM_PRINT_LOG.LogEvenF
#else
#	define LOGEVEN	G_COM_PRINT_LOG.LogEvenD
#endif //#ifdef DEBUG_LOG 

namespace COMMUSE
{
	/** 
	*   @ClassName	: ComPrintLog
	*   @Purpose	: ������־
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
		//��̬ʵ��
		static ComPrintLog& GetInstance()
		{
			static ComPrintLog c;
			return c;
		}
		//��debugview��ӡ
#	ifdef UNICODE
		void LogEvenD (wchar_t* format,...);
#	else
		void LogEvenD (char* format,...);
#	endif
		
		//��File��ӡ
#	ifdef UNICODE
		void LogEvenF (wchar_t* format,...);
#	else
		void LogEvenF (char* format,...);
#	endif
	protected:
	private:
		FILE *				_LogFile ;				//��ӡ�ļ����
		char				_szLogFilePath[MAX_PATH];//log�ļ�·��
		char				_szAppFileName[MAX_PATH];//Ӧ�ó�����

		CRITICAL_SECTION	_cs_log;				//��ӡ�ļ���
		//��ʼ��
		void PrintInit();
		//����ʼ��
		void PrintUnInit();
	};
}


#endif //__CommLog_H




