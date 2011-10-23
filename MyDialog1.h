#pragma once
#include "afxcmn.h"

// CMyDialog1 对话框

class CMyDialog1 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog1)

public:
	CMyDialog1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDialog1();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl1;
	
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
