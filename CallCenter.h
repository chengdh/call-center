// Callcenter.h : Callcenter Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
#endif

#include "resource.h"       // ������


// CCallCenterApp:
// �йش����ʵ�֣������ Callcenter.cpp
//

class CCallCenterApp : public CWinApp
{
public:
	CCallCenterApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CCallCenterApp theApp;
