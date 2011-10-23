// MyDialog3.cpp : 实现文件
//

#include "stdafx.h"
#include "Callcenter.h"
#include "MyDialog3.h"
#include ".\mydialog3.h"
#include "CTIServer.h"


// CMyDialog3 对话框

IMPLEMENT_DYNAMIC(CMyDialog3, CDialog)
CMyDialog3::CMyDialog3(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDialog3::IDD, pParent)
{
}

CMyDialog3::~CMyDialog3()
{
}

void CMyDialog3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_ListCtrl3);
	DDX_Control(pDX, IDC_CHECKDATE, m_ctrlCheckDate);
	DDX_Control(pDX, IDC_STARTDATE, m_ctrlStartDate);
	DDX_Control(pDX, IDC_ENDDATE, m_ctrlEndDate);
	DDX_Control(pDX, IDC_DELDATE, m_ctrlDelDate);
}


BEGIN_MESSAGE_MAP(CMyDialog3, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_RADIO_DAY, OnBnClickedRadioDay)
	ON_BN_CLICKED(IDC_RADIO_DISTANCE, OnBnClickedRadioDistance)
	ON_NOTIFY(LVN_HOTTRACK, IDC_LIST3, OnLvnHotTrackList3)
END_MESSAGE_MAP()


// CMyDialog3 消息处理程序
BOOL CMyDialog3::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//
	m_ListCtrl3.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD /*| LVS_EX_INFOTIP | LVS_EX_CHECKBOXES*/ );
	m_ListCtrl3.InsertColumn(0,_T("序号"),LVCFMT_CENTER,50);
	m_ListCtrl3.InsertColumn(1,_T("呼叫类型"),LVCFMT_CENTER,80);
	m_ListCtrl3.InsertColumn(2,_T("通道号码"),LVCFMT_CENTER,80);
	m_ListCtrl3.InsertColumn(3,_T("起始时间"),LVCFMT_CENTER,120);
	m_ListCtrl3.InsertColumn(4,_T("结束时间"),LVCFMT_CENTER,120);
	m_ListCtrl3.InsertColumn(5,_T("详细内容"),LVCFMT_CENTER,250);

	CButton *pButton1;
	pButton1 = (CButton*)GetDlgItem(IDC_RADIO_DAY);
	pButton1->SetCheck(TRUE);

	m_ctrlStartDate.EnableWindow(FALSE);
	m_ctrlEndDate.EnableWindow(FALSE);	

	m_ToolTip.Create(this);
	m_ToolTip.SetMaxTipWidth(500);
	m_ToolTip.AddTool(&m_ListCtrl3);
	m_ToolTip.SetDelayTime(500);
	m_ToolTip.Activate(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyDialog3::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnOK();
}

void CMyDialog3::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnCancel();
}

void CMyDialog3::OnBnClickedButtonCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ListCtrl3.DeleteAllItems();
	CButton *pButton1,*pButton2;
	pButton1 = (CButton*)GetDlgItem(IDC_RADIO_DAY);
	pButton2 = (CButton*)GetDlgItem(IDC_RADIO_DISTANCE);
	if( pButton1->GetCheck() )
	{
		CTime ct1;
		CString strTime;
		CString strFile;
		CFileFind fileFind;

		m_ctrlCheckDate.GetTime(ct1);		
		strTime = ct1.Format(_T("%Y-%m-%d"));
		strFile = CString(_T(".\\Log\\")) + strTime + CString(_T(".xml"));
		BOOL isFinded = (BOOL)fileFind.FindFile(strFile);
		if( !isFinded )
		{
			AfxMessageBox(_T("日志文件不存在。"));
			return;
		}
		myXml.showLogByDay(ct1,&m_ListCtrl3);
	}
	else if( pButton2->GetCheck() )
	{
		CTime ct1,ct2;
		m_ctrlStartDate.GetTime(ct1);
		m_ctrlEndDate.GetTime(ct2);
		if( ct1 > ct2 )
		{
			AfxMessageBox(_T("截止时间早于起始时间，请重新选择。"));
			return;
		}
		else
		{
			myXml.showLogByDistance(ct1,ct2,&m_ListCtrl3);
		}
	}
}

void CMyDialog3::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	for( int i = 0;i<pCTIServer->TotalLine;i++ )
	{
		if( pCTIServer->Lines[i].UsableFlag == FALSE)
		{
			AfxMessageBox(_T("部分日志可能正在使用,请等待所有通道空闲后再进行删除."));
			return;
		}
	}
	CTime ct1,ct2;
	m_ctrlDelDate.GetTime(ct1);
	ct2 = CTime::GetCurrentTime();
	if( ct1 > ct2 || ct1.GetYear() == ct2.GetYear() && ct1.GetMonth() == ct2.GetMonth() && ct1.GetDay() == ct2.GetDay() )
	{
		AfxMessageBox(_T("截止日期不能为当天及其以后日期。"));
		return;
	}

	CString strTime;
	CString strFile;
	CString fileName;
	CFileFind fileFind;

	strTime = ct1.Format(_T("%Y-%m-%d")) + CString(_T(".xml"));
	strFile = CString(_T(".\\Log\\*.*"));
	BOOL isFinded = (BOOL)fileFind.FindFile(strFile);

	while(isFinded)
	{
		isFinded = (BOOL)fileFind.FindNextFile();
		if( !fileFind.IsDots() && !fileFind.IsDirectory() )
		{
			fileName = fileFind.GetFileName();
			if( fileName <= strTime )
			{
				strFile = CString(_T(".\\Log\\")) + fileName;
				DeleteFile(strFile);
			}
		}	
	}
	AfxMessageBox(_T("日志删除完成。"));
	
	//更新界面
	m_ListCtrl3.DeleteAllItems();
	ct2 = CTime::GetCurrentTime();
	strTime = ct2.Format(_T("%Y-%m-%d")) + CString(_T(".xml"));
	strFile = CString(_T(".\\Log\\")) + strTime; 
	isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
		AfxMessageBox(_T("日志文件不存在。"));
		return;
	}
	myXml.showLogByDay(ct2,&m_ListCtrl3);

}

void CMyDialog3::OnBnClickedRadioDay()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_ctrlCheckDate.EnableWindow(TRUE);
	m_ctrlStartDate.EnableWindow(FALSE);
	m_ctrlEndDate.EnableWindow(FALSE);
}

void CMyDialog3::OnBnClickedRadioDistance()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlStartDate.EnableWindow(TRUE);
	m_ctrlEndDate.EnableWindow(TRUE);
	m_ctrlCheckDate.EnableWindow(FALSE);

}

BOOL CMyDialog3::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_ToolTip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void CMyDialog3::OnLvnHotTrackList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CListCtrl* pList = (CListCtrl*) GetDlgItem(IDC_LIST3);
    int nSelected = pNMLV->iItem;
	int nSubItem = pNMLV->iSubItem;
	CRect rect,rect2;
    if( nSelected >= 0 && nSubItem >= 0 ) 
	{
		CString strText = pList->GetItemText(nSelected,nSubItem);
		m_ToolTip.UpdateTipText(strText,pList);
	}

	*pResult = 0;
}

void CMyDialog3::setCTIServer(CTIServer *cs)
{
	pCTIServer = cs;
}

