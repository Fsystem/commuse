
// WinCrc32Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WinCrc32.h"
#include "WinCrc32Dlg.h"
#include "crc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinCrc32Dlg �Ի���




CWinCrc32Dlg::CWinCrc32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinCrc32Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinCrc32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinCrc32Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CWinCrc32Dlg ��Ϣ�������

BOOL CWinCrc32Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWinCrc32Dlg::OnPaint()
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
HCURSOR CWinCrc32Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWinCrc32Dlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	char sPath[_MAX_PATH]={0};
	int nFile = DragQueryFile(hDropInfo,0,sPath,_MAX_PATH);

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
			delete[] pbuf;
			GetDlgItem(IDC_EDIT_CRC)->SetWindowText(sCrc);
			fclose(fp);
		}
	}
	CDialog::OnDropFiles(hDropInfo);
}
