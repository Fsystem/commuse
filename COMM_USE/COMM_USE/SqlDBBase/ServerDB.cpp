#include "StdAfx.h"
#include "ServerDB.h"


ServerDB::ServerDB(int nDBConnectCont,LPCTSTR defDB,LPCTSTR dbUser,LPCTSTR dbPwd,DWORD dbPort)
{
	BEGIN_FUNC
	_init  =false;
	ASSERT(dbUser); if(!dbUser) return;
	ASSERT(dbPwd); if(!dbPwd) return;
	ASSERT(defDB); if(!defDB) return;
	ASSERT(0 < nDBConnectCont && 100 > nDBConnectCont); if( 0 >= nDBConnectCont || 100 <= nDBConnectCont) return;
	
	_nDBConnectCount	= nDBConnectCont;
	lstrcpyn(_defDb,defDB,255);
	_defDb[255] = 0;
	lstrcpyn(_tszUser,dbUser,32);
	_tszUser[32]=0;
	lstrcpyn(_tszPwd,dbPwd,32);
	_tszPwd[32]=0;
	_port = dbPort;

	CoInitialize(NULL);

	//连接数据库
	wsprintf(_tszConnStr,_T("Driver=SQL Server;Server=.;Database=%s;UID=%s;PWD=%s"),
		_defDb,_tszUser,_tszPwd);
	_pCon = new _ConnectionPtr[_nDBConnectCount];

	
	try
	{
		
		for(int i = 0 ; i<_nDBConnectCount;i++)
		{
			if ( FAILED(_pCon[i].CreateInstance(__uuidof(Connection)) ) )
			{
				ASSERT(false);
				return;
			}

			_pCon[i]->Open((_bstr_t)_tszConnStr,_T(""),_T(""),adModeUnknown);
			_pCon[i]->CursorLocation = (adUseClient);
			_freeDbConnectList.push_back(_pCon + i);
		}
		
		_init  = true;
	}
	catch (_com_error& e)
	{
		AfxMessageBox(e.Description());
		_init  =false;
	}
	
	END_FUNC
}


ServerDB::~ServerDB()
{
	BEGIN_FUNC
	if (_init)
	{
		_init = false;
		if (_pCon)
		{
			for(int i = 0 ; i<_nDBConnectCount;i++)
			{
				_pCon[i]->Close();
				_pCon[i].Release();
			}
			delete[] _pCon;
			_pCon = NULL;
		}
		CoUninitialize();
	}
	END_FUNC
}

//检测连接是否正常
void ServerDB::CheckConnect(_ConnectionPtr& con)
{
	BEGIN_FUNC
	if (_init)
	{
		TCHAR tszSqlCon[256]={0};
		wsprintf(_tszConnStr,_T("Driver=SQL Server;Server=.;Database=%s;UID=%s;PWD=%s"),
			_defDb,_tszUser,_tszPwd);
		
		while( adStateOpen != con->GetState() )
		{
			try
			{
				con->Close();
				con->Open( (_bstr_t)_tszConnStr,_T(""),_T(""),adModeUnknown);
			}
			catch(_com_error& e)
			{
				TRACE((char*)e.Description());
				LOGEVEN((TCHAR*)e.Description());
				ASSERT(false);
			}
		}
	}
	END_FUNC
}


_ConnectionPtr* ServerDB::GetConnect()
{
	_ConnectionPtr* p = NULL;
	if (_init)
	{
		while (0 >= _freeDbConnectList.size() ) Sleep(1);
		LOCK lk(_cs_db);
		LOGEVEN(_T("[DB]当前空闲数据库连接[%d]\n"),_freeDbConnectList.size() );
		p = _freeDbConnectList.front();
		_freeDbConnectList.pop_front();
	}
	
	return p;
}

 
bool ServerDB::ExecuteSql(LPCTSTR sql,_RecordsetPtr& record ,CommandTypeEnum typ,long nParamCount,tagSqlParam* pParam )
{
	_ConnectionPtr* pCon = NULL;
	BEGIN_FUNC
	if (_init)
	{
		_ConnectionPtr _con;
		if( (pCon = GetConnect()) )
		{
			_con = *pCon;
			CheckConnect(_con);

			try
			{
				_CommandPtr cmd(__uuidof(Command));
				cmd->ActiveConnection=_con;
				cmd->CommandText=_bstr_t(sql);
				cmd->CommandType = typ;
				if ( 0 < nParamCount && pParam )
				{
					cmd->Parameters->Refresh();
					for( long l = 0L ; l < nParamCount ; l++)
					{
						cmd->Parameters->GetItem( long(l+1) )->Value=pParam[l].vParam;
					}
				}

				record = cmd->Execute(NULL,NULL,typ);

				if ( 0 < nParamCount && pParam )
				{
					for( long l = 0L ; l < nParamCount ; l++)
					{
						if ( pParam[l].bIsResult )
						{
							pParam[l].vParam = cmd->Parameters->GetItem( long(l+1) )->GetValue();
						}
					}
				}

				// 				_ParameterPtr Parameter;
				// 				long lParameterCount=cmd->Parameters->Count;
				// 				for (long i=0;i<lParameterCount;i++)
				// 				{
				// 					Parameter=cmd->Parameters->Item[i];
				// 					if (Parameter->Direction==adParamReturnValue) return Parameter->Value.lVal;
				// 				}


				LOCK lk(_cs_db);
				_freeDbConnectList.push_back(pCon);

				return true;
			}
			catch(_com_error& e)
			{
				TRACE((char*)e.Description());
				ASSERT(false);
				LOCK lk(_cs_db);
				_freeDbConnectList.push_back(pCon);
				return false;
			}

		}



		
	}

	END_FUNC

	if(pCon)
	{
		LOCK lk(_cs_db);
		_freeDbConnectList.push_back(pCon);
	}
	
	return false;
}