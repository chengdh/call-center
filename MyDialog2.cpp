// MyDialog2.cpp : 实现文件
//

#include "stdafx.h"
#include "Callcenter.h"
#include "MyDialog2.h"
#include "EmployeeAdd.h"
#include "EmployeeModify.h"
#include "CTIServer.h"

// CMyDialog2 对话框

IMPLEMENT_DYNAMIC(CMyDialog2, CDialog)
CMyDialog2::CMyDialog2(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDialog2::IDD, pParent)
{
	m_nCurSel=-1;
	//	m_strConn="Provider=sqloledb;Data Source=.;Initial Catalog=DB;User Id=sa;Password=;";
	m_strConn="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=.\\DB.mdb";
}

CMyDialog2::~CMyDialog2()
{	
}

void CMyDialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl2);
}


BEGIN_MESSAGE_MAP(CMyDialog2, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CMyDialog2::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTONADD, &CMyDialog2::OnBnClickedButtonadd)
	ON_BN_CLICKED(IDC_BUTTONMODIFY, &CMyDialog2::OnBnClickedButtonmodify)
	ON_BN_CLICKED(IDC_BUTTONDEL, &CMyDialog2::OnBnClickedButtondel)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, &CMyDialog2::OnNMClickList2)
	ON_BN_CLICKED(IDC_BUTTONUPDATE, &CMyDialog2::OnBnClickedButtonupdate)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMyDialog2 消息处理程序
BOOL CMyDialog2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

    m_ListCtrl2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD /*| LVS_EX_CHECKBOXES  */ /*| LVS_EX_INFOTIP | LVS_EX_CHECKBOXES*/ );
	m_ListCtrl2.InsertColumn(0,_T("序号"),LVCFMT_CENTER,40);
	m_ListCtrl2.InsertColumn(1,_T("用户代码"),LVCFMT_CENTER,80);
	m_ListCtrl2.InsertColumn(2,_T("姓名"),LVCFMT_CENTER,100);
	m_ListCtrl2.InsertColumn(3,_T("分机号码"),LVCFMT_CENTER,80);
	m_ListCtrl2.InsertColumn(4,_T("通道号码"),LVCFMT_CENTER,80);
	m_ListCtrl2.InsertColumn(5,_T("最近登录时间"),LVCFMT_CENTER,120);
	m_ListCtrl2.InsertColumn(6,_T("最近退出时间"),LVCFMT_CENTER,120);

	//	
	_RecordsetPtr pSet;
	try
	{
		pConn = mySql.openConnection(_com_util::ConvertStringToBSTR(m_strConn),"","",adOpenUnspecified);
		pSet = mySql.getUserDetails(pConn);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	pSet->MoveFirst();
	int i=0;
	CString iStr;
	_variant_t varTemp;
	while(!pSet->EndOfFile)
	{
		iStr.Format(_T("%d"),i);
		m_ListCtrl2.InsertItem(i,iStr.Trim());
		m_ListCtrl2.SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("UserId"))));
		m_ListCtrl2.SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("Name"))));
		
		_variant_t varTemp;
		varTemp = pSet->GetCollect(_variant_t("RecentLogin"));
		if( varTemp.vt != VT_NULL )
			m_ListCtrl2.SetItemText(i,5,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		varTemp = pSet->GetCollect(_variant_t("RecentLogout"));
		if( varTemp.vt != VT_NULL )
			m_ListCtrl2.SetItemText(i,6,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		
		i++;
		pSet->MoveNext();
	} 
	pSet->Close();
	pSet=NULL;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyDialog2::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here	
	
	*pResult = 0;
}

void CMyDialog2::OnBnClickedButtonadd()
{
	CEmployeeAdd empAddDlg;
	empAddDlg.setList(&m_ListCtrl2);
	empAddDlg.DoModal();
	displayPhoneNo();
}

void CMyDialog2::OnBnClickedButtonmodify()
{
	// TODO: Add your control notification handler code here
	CListCtrl* pList = (CListCtrl*) GetDlgItem(IDC_LIST2);
	POSITION pos= pList->GetFirstSelectedItemPosition();
	if(pos == NULL)
		AfxMessageBox(_T("请选择一行。"));
	else
	{
		int nItem = pList->GetNextSelectedItem(pos); 
		CEmployeeModify empModifyDlg;
		empModifyDlg.setList(pList,nItem);
		empModifyDlg.DoModal();
		displayPhoneNo();
	}
}

void CMyDialog2::OnBnClickedButtondel()
{
	CListCtrl* pList =( CListCtrl*)GetDlgItem(IDC_LIST2);
	int j, itemCount, nItem;
	j = 0;
	nItem = -1;
	CString str="";

	POSITION pos= pList->GetFirstSelectedItemPosition();
	if(pos == NULL)
	{
		AfxMessageBox(_T("请先选择一行。"));
		return;
	}

    nItem=pList->GetNextSelectedItem(pos);
    CString strNo = pList->GetItemText(nItem,1);
	CString strPhone = _T("");
	int no = -1;

	if( pCTIServer->checkLoginByUserId(strNo,strPhone,no) )
	{
		AfxMessageBox(_T("此用户已经登录,不可删除."));
		return;
	}

	CString str1 = CString("确定删除用户") + strNo + CString(" ？") ;	
	if(AfxMessageBox(str1,MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}
	pList->DeleteItem(nItem);	
	itemCount=pList->GetItemCount();
	for(int i=0;i<itemCount;i++)
	{
		str.Format(_T("%d"),i);
		pList->SetItemText(i,0,str);
	}

	_variant_t RecordsAffected = 0;
	CString strSqlDelete="";	
	strSqlDelete.Format(_T("delete from UserDetails where UserId='%s';"),strNo.Trim());	//不可缺少from
	try
	{
		pConn->Execute(strSqlDelete.AllocSysString(),&RecordsAffected,adCmdText);				
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
}

void CMyDialog2::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;
}

void CMyDialog2::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnOK();
}

void CMyDialog2::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnCancel();
}

void CMyDialog2::OnBnClickedButtonupdate()
{
	// TODO: Add your control notification handler code here
	m_ListCtrl2.DeleteAllItems();
	_RecordsetPtr pSet;
	pSet = mySql.getUserDetails(pConn);
	pSet->MoveFirst();
	int i=0;
	CString iStr;
	while(!pSet->EndOfFile)
	{
		iStr.Format(_T("%d"),i);
		m_ListCtrl2.InsertItem(i,iStr);
		m_ListCtrl2.SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("UserId"))));
		m_ListCtrl2.SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("Name"))));
		
		_variant_t varTemp;
		varTemp = pSet->GetCollect(_variant_t("RecentLogin"));
		if( varTemp.vt != VT_NULL )
			m_ListCtrl2.SetItemText(i,5,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		varTemp = pSet->GetCollect(_variant_t("RecentLogout"));
		if( varTemp.vt != VT_NULL )
			m_ListCtrl2.SetItemText(i,6,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		
		i++;
		pSet->MoveNext();
	} 
	pSet->Close();
	pSet=NULL;	
	//
	displayPhoneNo();
}

void CMyDialog2::setCTIServer(CTIServer *cs)
{
	pCTIServer = cs;
}

void CMyDialog2::displayPhoneNo()
{
	int itemCount = m_ListCtrl2.GetItemCount();
	if( itemCount == 0 )
		return;
	CString strUserId = _T("");
	CString strPhone = _T("");
	int no = -1;
	CString strNo = _T("");
	for( int j = 0; j < itemCount; j++ )
	{
		strUserId = m_ListCtrl2.GetItemText(j,1);
		if( pCTIServer->checkLoginByUserId(strUserId,strPhone,no) )
		{
			TRACE3("\nstrUserId=%s,strPhone=%s,no=%d\n",strUserId,strPhone,no);
			m_ListCtrl2.SetItemText(j,3,strPhone);
			strNo.Format("%d",no);
			m_ListCtrl2.SetItemText(j,4,strNo);
		}
	}
}
