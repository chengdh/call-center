#pragma once
#include "afxcmn.h"
#include "MySql.h"

class CTIServer;
// CMyDialog2 �Ի���

class CMyDialog2 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog2)

public:
	CMyDialog2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyDialog2();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl2;
	int m_nCurSel;
	CString m_strConn;
	MySql mySql;
	_ConnectionPtr pConn;
	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	
public:
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonadd();
	afx_msg void OnBnClickedButtonmodify();
	afx_msg void OnBnClickedButtondel();
	afx_msg void OnBnClickedButtonupdate();
	
protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	CTIServer *pCTIServer;
	void setCTIServer(CTIServer* cs);
	void displayPhoneNo();
};

