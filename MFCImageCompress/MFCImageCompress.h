
// MFCImageCompress.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCImageCompressApp: 
// �йش����ʵ�֣������ MFCImageCompress.cpp
//

class CMFCImageCompressApp : public CWinApp
{
public:
	CMFCImageCompressApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCImageCompressApp theApp;