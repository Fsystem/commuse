
// MakeUpdateListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MakeUpdateList.h"
#include "MakeUpdateListDlg.h"
#include "crc.h"

#pragma comment(lib,"libCrc32.lib")



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMakeUpdateListDlg 对话框




CMakeUpdateListDlg::CMakeUpdateListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeUpdateListDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	
	m_ExePath = GetExecuteFilePath();
	m_ExeName = GetExecuteFileName();
	m_XmlName = "DownLoad.xml";
}

CMakeUpdateListDlg::~CMakeUpdateListDlg()
{
	
}

void CMakeUpdateListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeUpdateListDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CMakeUpdateListDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMakeUpdateListDlg 消息处理程序

BOOL CMakeUpdateListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMakeUpdateListDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMakeUpdateListDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMakeUpdateListDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CMakeUpdateListDlg::IsRoot(LPCTSTR lpszPath) 
{ 
	TCHAR szRoot[4]; 
	wsprintf(szRoot, "%c:\\", lpszPath[0]);         
	return (lstrcmp(szRoot, lpszPath) == 0); 
} 


void CMakeUpdateListDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();

	//创建一个XML的文档对象。
	m_pXmlDoc = new TiXmlDocument();
	//创建xml申明
	TiXmlDeclaration Dec( "1.0","utf-8", "yes");
	m_pXmlDoc->InsertEndChild(Dec);
	//创建一个根元素并连接。
	m_pRootNode = new TiXmlElement("Resources");
	m_pXmlDoc->LinkEndChild(m_pRootNode);
	
	FindAll(m_ExePath);
	MakeXmlFile(m_XmlName.GetBuffer());

	if (m_pXmlDoc)
	{
		delete m_pXmlDoc;
		m_pXmlDoc=NULL;
	}

	AfxMessageBox("生成成功");
}

void CMakeUpdateListDlg::FindAll(LPCTSTR szPath)
{
	char* szFilter = "*.*";

	char szFind[MAX_PATH]; 
	strcpy(szFind, szPath); 
	if (!IsRoot(szFind)) 
		lstrcat(szFind, "\\"); 
	strcat(szFind, szFilter); // 找所有文件 
	WIN32_FIND_DATA wfd; 
	HANDLE hFind = FindFirstFile(szFind, &wfd); 
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败 
		return; 

	do 
	{ 
		if (wfd.cFileName[0] == '.' || 0==strcmp(m_ExeName.GetBuffer(),wfd.cFileName) ||
			0==strcmp(m_XmlName.GetBuffer(),wfd.cFileName) ) 
			continue; // 过滤这两个目录('.' '..' ) 
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{ 
 			char szFile[MAX_PATH]; 
 			if (IsRoot(szPath)) 
 				sprintf(szFile, "%s%s", szPath, wfd.cFileName); 
 			else 
 			{
 				sprintf(szFile, "%s\\%s", szPath, wfd.cFileName); 
 				FindAll(szFile); // 如果找到的是目录，则进入此目录进行递归 
 			}
		} 
		else 
		{ 
 			char szFile[MAX_PATH]; 
 			if (IsRoot(szPath)) 
 			{
 				sprintf(szFile, "%s%s", szPath, wfd.cFileName); 
 			}
 			else 
 			{
 				sprintf(szFile, "%s\\%s", szPath, wfd.cFileName); 
 				//printf("%s\n",szFile); 
 			}
			// 对文件进行操作 

			MakeNode( wfd.cFileName,GetCrc(szFile),IsZip(szFile),
				GetZipFileName(szFile),GetZipFilePath(szFile),GetFilePath(szFile));
	
		} 
	} while (FindNextFile(hFind, &wfd)); 
	FindClose(hFind); // 关闭查找句柄
}

string CMakeUpdateListDlg::GetCrc(string strFilePath)
{
	string sCrc="";
	char szCrc32[9]={0};
	FILE* fp=fopen(strFilePath.c_str(),"rb");
	if (fp)
	{
		fseek(fp,0,SEEK_END);
		int nFileSize = ftell(fp);
		fseek(fp,0,SEEK_SET);

		char* pData=new char[nFileSize];
		fread(pData,nFileSize,1,fp);
		fclose(fp);

		sprintf(szCrc32,"%X",GetCrcLong(pData,nFileSize));
		delete[] pData;
	}
	sCrc=szCrc32;

	return sCrc;
}

string CMakeUpdateListDlg::IsZip(string strFilePath)
{
	string strZip="0";
	char* pSz=(char*)strFilePath.c_str();
	if( strstr(pSz,"\\zip\\") ) strZip="1";
	return strZip;
}

string CMakeUpdateListDlg::GetZipFileName(string strFilePath)
{
	string str="null";
	if ("1"==IsZip(strFilePath))
	{
		string strArr[MAX_PATH]={""};
		int nNum=0;
		SpliteStr(strFilePath,'\\',strArr,nNum);
		int nZipIndex=-1;
		for (int i=0;i<nNum;i++)
		{
			if ("zip"==strArr[i])
			{
				nZipIndex=i;
			}
		}
		if (-1 != nZipIndex && nZipIndex < nNum-1 )
		{
			str=strArr[nZipIndex+1];
		}
	}
	return str;
}

string CMakeUpdateListDlg::GetZipFilePath(string strFilePath)
{
	string str="null";
	if ("1"==IsZip(strFilePath))
	{
		string strArr[MAX_PATH]={""};
		int nNum=0;
		SpliteStr(strFilePath,'\\',strArr,nNum);
		int nZipIndex=-1;
		for (int i=0;i<nNum;i++)
		{
			if ("zip"==strArr[i])
			{
				nZipIndex=i;
			}
		}
		if (-1 != nZipIndex && nZipIndex < nNum-1 && 0 < nZipIndex )
		{
			str=strArr[nZipIndex-1]+"/"+strArr[nZipIndex+1];
		}
	}
	return str;
}

string CMakeUpdateListDlg::GetFilePath(string strFilePath)
{
	string str="";
	string strArr[MAX_PATH]={""};
	int nNum=0;
	SpliteStr(strFilePath,'\\',strArr,nNum);

	string sFileName="";
	int nIndex=strFilePath.find_last_of('\\');
	if (-1 != nIndex)
	{
		sFileName=strFilePath.substr(nIndex+1,strFilePath.size()-nIndex);
	}

	if ("1"==IsZip(strFilePath))
	{
		int nZipIndex=-1;
		for (int i=0;i<nNum;i++)
		{
			if ("zip"==strArr[i])
			{
				nZipIndex=i;
			}
		}
		if (-1 != nZipIndex && nZipIndex < nNum-1 && 0 < nZipIndex )
		{
			string sFileEx="";
			for (int i=nZipIndex+2;i<nNum;i++)
			{
// 				int nDotIndex=strArr[i].find_first_of('.');
// 				if (-1 != nDotIndex && i<nNum-1 )
// 				{
// 					sFileEx+=strArr[i].substr(0,nDotIndex);
// 				}
// 				else
// 				{
// 					sFileEx+=strArr[i];
// 				}
				sFileEx+=strArr[i];
				if ( i != nNum-1)
				{
					sFileEx+="/";
				}
				
				
			}
			str=sFileEx;
		}

	}
	else
	{
		//CString sNotUseDic=m_ExePath+_T("\\");
		//CString sFullFileName=CString(strFilePath.c_str());
		//sFullFileName.Replace(sNotUseDic,_T(""));

		string sNotUseDic=m_ExePath+"\\";
		string sFullFileName=strFilePath;
		sFullFileName.replace(0,sNotUseDic.length(),"");
		//TRACE(_T(">>>***********************************************\n"));
		//TRACE(_T(">>>:::%s\n"),sFullFileName);
		for (int i=0;i<sFullFileName.length();i++)
		{
			if ('\\'==sFullFileName[i])sFullFileName[i]='/';
		}

		str=sFullFileName;

	}
// 	TRACE(_T(">>>:::%s\n"),strFilePath.c_str());
// 	TRACE(_T(">>>:::%s\n"),m_ExePath);
// 	TRACE(_T(">>>:::%s\n"),m_ExeName);
	TRACE(_T(">>>:::%s\n"),str.c_str());
	TRACE(_T(">>>========================================\n"));
	return str;
}

void CMakeUpdateListDlg::SpliteStr(string strFull,char cFilter,string* pStrArr,int& nOutNum)
{
	nOutNum=0;
	ASSERT(pStrArr);
	if (pStrArr)
	{
		int nFirstPos=0,nLastPos=0;
		for (int i=0;i<strFull.size();i++)
		{
			nLastPos=i;
			if (strFull[i]==cFilter)
			{
				pStrArr[nOutNum++]=strFull.substr(nFirstPos,nLastPos-nFirstPos);
				nFirstPos=nLastPos+1;
			}
		}
		if (nFirstPos!=nLastPos)
		{
			pStrArr[nOutNum++]=strFull.substr(nFirstPos,nLastPos-nFirstPos+1);
			nFirstPos=nLastPos;
		}
	}
}

CString CMakeUpdateListDlg::GetExecuteFilePath()
{
	CString strPath="";
	char szPath[MAX_PATH]={0};
	GetModuleFileNameA(NULL,szPath,MAX_PATH);
	strPath=CString(szPath);
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	return strPath;
}

CString CMakeUpdateListDlg::GetExecuteFileName()
{
	CString strName="";
	char szPath[MAX_PATH]={0};
	GetModuleFileNameA(NULL,szPath,MAX_PATH);
	strName=CString(szPath);
	strName = strName.Right(strName.GetLength()-strName.ReverseFind('\\')-1);
	return strName;
}

bool CMakeUpdateListDlg::MakeNode(string strNode,string strCrc,string strZip,string strZIPFileName,
								  string strZIPFilePath,string strfilePath)
{
	try
	{
		TiXmlElement* pXmlNode = new TiXmlElement("file");
		m_pRootNode->LinkEndChild(pXmlNode);

		string strName="name";
		pXmlNode->SetAttribute(strName.c_str(),strNode.c_str());

		TiXmlElement* pXmlNodeCrc			= new TiXmlElement("CRC");
		TiXmlElement* pXmlNodeZip			= new TiXmlElement("ZIP");
		TiXmlElement* pXmlNodeZipName		= new TiXmlElement("ZIPFileName");
		TiXmlElement* pXmlNodeZipFilePath	= new TiXmlElement("ZIPFilePath");
		TiXmlElement* pXmlNodeFilePath		= new TiXmlElement("filePath");

		pXmlNode->LinkEndChild(pXmlNodeCrc);
		pXmlNode->LinkEndChild(pXmlNodeZip);
		pXmlNode->LinkEndChild(pXmlNodeZipName);
		pXmlNode->LinkEndChild(pXmlNodeZipFilePath);
		pXmlNode->LinkEndChild(pXmlNodeFilePath);


		TiXmlText* pTextCrc = new TiXmlText(strCrc.c_str());
		pXmlNodeCrc->LinkEndChild(pTextCrc);

		TiXmlText* pTextZip = new TiXmlText(strZip.c_str());
		pXmlNodeZip->LinkEndChild(pTextZip);

		TiXmlText* pTextZIPFileName = new TiXmlText(strZIPFileName.c_str());
		pXmlNodeZipName->LinkEndChild(pTextZIPFileName);

		TiXmlText* pTextZIPFilePath = new TiXmlText(strZIPFilePath.c_str());
		pXmlNodeZipFilePath->LinkEndChild(pTextZIPFilePath);

		TiXmlText* pTextfilePath = new TiXmlText(strfilePath.c_str());
		pXmlNodeFilePath->LinkEndChild(pTextfilePath);

// 		delete pTextCrc;
// 		delete pTextZip;
// 		delete pTextZIPFileName;
// 		delete pTextZIPFilePath;
// 
// 
// 		delete pXmlNodeZip;
// 		delete pXmlNodeZipName;
// 		delete pXmlNodeZipFilePath;
// 		delete pXmlNodeFilePath;
// 		delete pXmlNodeCrc;
// 
// 		delete pXmlNode;
	}
	catch(string& e)
	{
		MessageBoxA(e.c_str());
		return false;
	}
	

	return true;
}

bool CMakeUpdateListDlg::MakeXmlFile(string strFile)
{
	try
	{
		string seperator = "\\";
		string fullPath = m_ExePath.GetBuffer(0) +seperator+strFile;
		m_pXmlDoc->SaveFile(fullPath.c_str());//保存到文件
	}
	catch (string& e)
	{
		MessageBoxA(e.c_str());
		return false;
	}
	return true;
}
