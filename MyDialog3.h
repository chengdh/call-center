#pragma once
#include "afxcmn.h"
#include "afxdtctl.h"
#include "MyXML.h"

class CTIServer;
// CMyDialog3 对话框

class CMyDialog3 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog3)

public:
	CMyDialog3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyDialog3();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl3;
	MyXML myXml;
	CToolTipCtrl m_ToolTip;
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	CTIServer *pCTIServer;
	void setCTIServer(CTIServer* cs);

public:
	CDateTimeCtrl m_ctrlCheckDate;
	CDateTimeCtrl m_ctrlStartDate;
	CDateTimeCtrl m_ctrlEndDate;
	CDateTimeCtrl m_ctrlDelDate;
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedRadioDay();
	afx_msg void OnBnClickedRadioDistance();
protected:
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLvnHotTrackList3(NMHDR *pNMHDR, LRESULT *pResult);
};
