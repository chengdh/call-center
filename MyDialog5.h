#pragma once
#include "afxcmn.h"
#include "afxdtctl.h"


// CMyDialog5 对话框

class CMyDialog5 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog5)

public:
	CMyDialog5(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDialog5();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl5;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CDateTimeCtrl m_ctrlCheckDate;
	CDateTimeCtrl m_ctrlStartDate;
	CDateTimeCtrl m_ctrlEndDate;
	CDateTimeCtrl m_ctrlStartTime;
	CDateTimeCtrl m_ctrlEndTime;
	CDateTimeCtrl m_ctrlDelDate;
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedRadioDay();
	afx_msg void OnBnClickedRadioDistance();
	afx_msg void OnNMDblclkList5(NMHDR *pNMHDR, LRESULT *pResult);
};
