/** ��ӡ��־
*   @FileName  : CommLog.h
*   @Author    : Double Sword
*   @date      : 2011-8-28
*   @Copyright : Copyright belong to Author and ZhiZunNet.CO.
*   @RefDoc    : CommLogOpt.ini
				 1.Ϊ�����Ƿ��ӡ���ļ���û�д��ļ�����ӡ
				 2.[CommLog]
					Print = [0|1|2] 0-����ӡ 1-Debugview��ӡ 2-�ļ���ӡ
*/
#ifndef __CommLog_H
#define __CommLog_H

#define G_COM_PRINT_LOG COMMUSE::ComPrintLog::GetInstance()

#define LOGEVENW G_COM_PRINT_LOG.LogEventW
#define LOGEVENA G_COM_PRINT_LOG.LogEventA

#ifdef UNICODE
#	define LOGEVEN  LOGEVENW
#else
#	define LOGEVEN  LOGEVENA
#endif

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

		//ini���ƴ�ӡ
		void InitLogState();
		int GetLogState(){return _nLogState;}

		void LogEventW(wchar_t* format,...);
		void LogEventA(char* format,...);

		//-------------------------------------------------------------------------------
		//��debugview��ӡ
		void LogEvenDW (wchar_t* message);
		void LogEvenDA (char* message);

		//��File��ӡ
		void LogEvenFW (wchar_t* message);
		void LogEvenFA (char* message);
	private:
		HANDLE				_LogFile ;					//��ӡ�ļ����
		char				_szLogFilePath[MAX_PATH];	//log�ļ�·��
		char				_szAppFileName[MAX_PATH];	//Ӧ�ó�����
		int					_nLogState;					//�Ƿ��ӡ��־
		char				_szLogStateIniPath[MAX_PATH];//ini�ļ�·��

		CRITICAL_SECTION	_cs_log;				//��ӡ�ļ���
		//��ʼ��
		void PrintInit();
		//����ʼ��
		void PrintUnInit();
	};
}


#endif //__CommLog_H




