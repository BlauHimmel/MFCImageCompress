
// MFCImageCompressDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CMFCImageCompressDlg �Ի���
class CMFCImageCompressDlg : public CDialogEx
{
// ����
public:
	CMFCImageCompressDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCIMAGECOMPRESS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString cstrPathName;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CProgressCtrl progressCtrl;
};

struct ThreadInfo
{
	CString cstrPathName;
	HWND hWnd;
	CProgressCtrl* progressCtrl;
};

UINT CompressProc(LPVOID pParam);