#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MySql.h"

// CMyDialog4 �Ի���

class CMyDialog4 : public CDialog
{
	DECLARE_DYNAMIC(CMyDialog4)

public:
	CMyDialog4(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyDialog4();

// �Ի�������
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	
public:
	CListCtrl m_ListCtrl4;
	CString m_strConn;
	MySql mySql;
	_ConnectionPtr pConn;
public:
	virtual BOOL OnInitDialog();
public:
	CComboBox m_ComboFreight;
	CComboBox m_ComboState;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	void displayDetailsByFreightState(CString type, _ConnectionPtr pConn);
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
