#pragma once
#include "MySql.h"

// CEmployeeModify 对话框

class CEmployeeModify : public CDialog
{
	DECLARE_DYNAMIC(CEmployeeModify)

public:
	CEmployeeModify(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEmployeeModify();

// 对话框数据
	enum { IDD = IDD_DIALOGMODIFYEMPLOYEE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonAddemp();
	afx_msg void OnBnClickedCancel1();	
	virtual BOOL OnInitDialog();

	int m_nAge;
	CString m_strNo;
	CString m_strName;
	CString m_strSex;
	CString m_strPwd1;
	CString m_strPwd2;

	CString m_strConn;
	MySql mySql;
	_ConnectionPtr pConn;
	CListCtrl *pList2;
	int nItem;

	HBRUSH m_hRedBrush;

public:
	void ResetControl();
	void setList(CListCtrl *listctrl,int nItem);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	virtual void OnOK();
};
