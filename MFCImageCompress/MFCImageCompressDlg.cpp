
// MFCImageCompressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCImageCompress.h"
#include "MFCImageCompressDlg.h"
#include "ImageCompress.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCImageCompressDlg �Ի���



CMFCImageCompressDlg::CMFCImageCompressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCIMAGECOMPRESS_DIALOG, pParent)
	, cstrPathName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCImageCompressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, cstrPathName);
	DDX_Control(pDX, IDC_PROGRESS1, progressCtrl);
}

BEGIN_MESSAGE_MAP(CMFCImageCompressDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCImageCompressDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCImageCompressDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCImageCompressDlg ��Ϣ�������

BOOL CMFCImageCompressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCImageCompressDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCImageCompressDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCImageCompressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CompressProc(LPVOID pParam)
{
	ThreadInfo* info = (ThreadInfo*)pParam;
	CString cstrPathName = info->cstrPathName;
	HWND hWnd = info->hWnd;
	CProgressCtrl* progressCtrl = info->progressCtrl;
	progressCtrl->SetRange(0, 100);
	progressCtrl->SetStep(1);
	g_progressCtrl = progressCtrl;

	CString cstrFileType = cstrPathName.Right(4);

	if (cstrFileType == ".bmp")
	{
		ImageCompress imageCompres;
		imageCompres.ReadBitmap(cstrPathName);

		CString cstrCompressPath = cstrPathName.Left(cstrPathName.GetLength() - 4) + ".img";
		imageCompres.Compress(cstrCompressPath);

		MessageBox(hWnd, _T("Compress succeeded!"), _T("INFO"), MB_OK);
	}
	else if (cstrFileType == ".img")
	{
		ImageCompress imageCompres;
		imageCompres.UnCompress(cstrPathName);

		CString cstrUncompressPath = cstrPathName.Left(cstrPathName.GetLength() - 4) + "-Uncompress.bmp";
		imageCompres.WriteBitmap(cstrUncompressPath);

		MessageBox(hWnd, _T("Uncompress succeeded!"), _T("INFO"), MB_OK);
	}
	else
	{
		MessageBox(hWnd, _T("File format must be \".bmp\" or \".img\"!"), _T("ERROR"), MB_OK);
	}

	g_progressCtrl = NULL;
	return 0;
}



void CMFCImageCompressDlg::OnBnClickedButton1()		//Open
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	CFileDialog dlg(true);

	if (dlg.DoModal() == IDOK)
	{
		cstrPathName = dlg.GetPathName();
	}

	UpdateData(false);
}


void CMFCImageCompressDlg::OnBnClickedButton2()		//Start
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	ThreadInfo* pThreadInfo = new ThreadInfo();
	pThreadInfo->cstrPathName = cstrPathName;
	pThreadInfo->hWnd = m_hWnd;
	pThreadInfo->progressCtrl = &progressCtrl;

	AfxBeginThread(CompressProc, pThreadInfo);

	UpdateData(false);
}
