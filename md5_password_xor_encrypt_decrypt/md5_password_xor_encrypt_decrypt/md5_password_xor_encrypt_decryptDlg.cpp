
// md5_password_xor_encrypt_decryptDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "md5_password_xor_encrypt_decrypt.h"
#include "md5_password_xor_encrypt_decryptDlg.h"
#include "Encrypt.h"
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Cmd5_password_xor_encrypt_decryptDlg �Ի���




Cmd5_password_xor_encrypt_decryptDlg::Cmd5_password_xor_encrypt_decryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cmd5_password_xor_encrypt_decryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//srand(GetTickCount());
}

void Cmd5_password_xor_encrypt_decryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edt_src_string);
	DDX_Control(pDX, IDC_EDIT2, m_edt_src_md5);
	DDX_Control(pDX, IDC_EDIT3, m_edt_src_password);
	DDX_Control(pDX, IDC_EDIT4, m_edt_src_xor);
	DDX_Control(pDX, IDC_EDIT5, m_edt_file);
	DDX_Control(pDX, IDC_EDIT6, m_edt_file_md5);
	DDX_Control(pDX, IDC_EDIT7, m_edt_file_crc32);
	DDX_Control(pDX, IDC_EDIT8, m_edt_xor_src);
	DDX_Control(pDX, IDC_EDIT9, m_edt_xor_decode);
}

BEGIN_MESSAGE_MAP(Cmd5_password_xor_encrypt_decryptDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1, &Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT8, &Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit8)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Cmd5_password_xor_encrypt_decryptDlg ��Ϣ�������

BOOL Cmd5_password_xor_encrypt_decryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cmd5_password_xor_encrypt_decryptDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cmd5_password_xor_encrypt_decryptDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cmd5_password_xor_encrypt_decryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString sSrcString;
	TCHAR szOut[1024]={0};
	m_edt_src_string.GetWindowText(sSrcString);
	if (0<sSrcString.GetLength())
	{
		memset(szOut,0,sizeof(szOut));
		CMD5Encrypt::EncryptData(sSrcString,szOut);
		m_edt_src_md5.SetWindowText(szOut);

		memset(szOut,0,sizeof(szOut));
		CMySqlPWEncrypt::EncryptData(sSrcString,szOut);
		m_edt_src_password.SetWindowText(szOut);

		memset(szOut,0,sizeof(szOut));
		CXOREncrypt::EncryptData(sSrcString,szOut,sizeof(szOut));
		m_edt_src_xor.SetWindowText(szOut);
	}
	else
	{
		m_edt_src_md5.SetWindowText(szOut);
		m_edt_src_password.SetWindowText(szOut);
		m_edt_src_xor.SetWindowText(szOut);
	}
}

void Cmd5_password_xor_encrypt_decryptDlg::OnEnChangeEdit8()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString sSrcString;
	TCHAR szOut[1024]={0};

	m_edt_xor_src.GetWindowText(sSrcString);

	if (0<sSrcString.GetLength())
	{
		CXOREncrypt::CrevasseData(sSrcString,szOut,sizeof(szOut));
		m_edt_xor_decode.SetWindowText(szOut);
	}
	else
	{
		m_edt_xor_decode.SetWindowText(szOut);
	}
}

void Cmd5_password_xor_encrypt_decryptDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	char sPath[_MAX_PATH]={0};
	int nFile = DragQueryFileA(hDropInfo,0,sPath,_MAX_PATH);

	if (0 < nFile )
	{
		FILE* fp = fopen(sPath,"rb");
		if (fp)
		{
			fseek(fp,0,SEEK_END);
			int nSize = ftell(fp);
			fseek(fp,0,SEEK_SET);
			char* pbuf = new char[nSize];
			fread(pbuf,1,nSize,fp);
			CString sCrc ;
			sCrc.Format(_T("%X"),GetCrcLong(pbuf,nSize) );
			TCHAR szOut[33]={0};
			CMD5Encrypt::EncryptData(pbuf,szOut);
			delete[] pbuf;
			//GetDlgItem(IDC_EDIT_CRC)->SetWindowText(sCrc);
			m_edt_file.SetWindowText(sPath);
			m_edt_file_crc32.SetWindowText(sCrc);
			m_edt_file_md5.SetWindowText(szOut);
			fclose(fp);

		}
	}

	CDialog::OnDropFiles(hDropInfo);
}
