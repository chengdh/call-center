#pragma once
#include "MySql.h"

// CEmployeeAdd �Ի���

class CEmployeeAdd : public CDialog
{
	DECLARE_DYNAMIC(CEmployeeAdd)

public:
	CEmployeeAdd(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEmployeeAdd();

// �Ի�������
	enum { IDD = IDD_DIALOGADDEMPLOYEE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonAddemp();
	afx_msg void OnBnClickedCancel1();	
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC *pDC,CWnd *pWnd,UINT nCtlColor);

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

	HBRUSH m_hRedBrush;

public:
	void ResetControl();
	void setList(CListCtrl *listctrl);
protected:
	virtual void OnOK();
};
