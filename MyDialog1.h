#pragma once
#include "afxcmn.h"

// CMyDialog1 �Ի���

class CMyDialog1 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog1)

public:
	CMyDialog1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyDialog1();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl1;
	
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
