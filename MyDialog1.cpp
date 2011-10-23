// MyDialog1.cpp : 实现文件
//

#include "stdafx.h"
#include "Callcenter.h"
#include "MyDialog1.h"


// CMyDialog1 对话框

IMPLEMENT_DYNAMIC(CMyDialog1, CDialog)
CMyDialog1::CMyDialog1(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDialog1::IDD, pParent)
{
}

CMyDialog1::~CMyDialog1()
{
}

void CMyDialog1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl1);
}


BEGIN_MESSAGE_MAP(CMyDialog1, CDialog)
END_MESSAGE_MAP()


// CMyDialog1 消息处理程序
BOOL CMyDialog1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//
	m_ListCtrl1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD /*| LVS_EX_INFOTIP | LVS_EX_CHECKBOXES*/ );
	m_ListCtrl1.InsertColumn(0,_T("通道号码"),LVCFMT_CENTER,60);
	m_ListCtrl1.InsertColumn(1,_T("通道类型"),LVCFMT_CENTER,80);
	m_ListCtrl1.InsertColumn(2,_T("流程状态"),LVCFMT_CENTER,100);
	m_ListCtrl1.InsertColumn(3,_T("主叫号码"),LVCFMT_CENTER,100);
	m_ListCtrl1.InsertColumn(4,_T("DTMF"),LVCFMT_CENTER,100);
	m_ListCtrl1.InsertColumn(5,_T("呼叫时间"),LVCFMT_CENTER,120);

	//
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyDialog1::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnOK();
}

void CMyDialog1::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnCancel();
}


