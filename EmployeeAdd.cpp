// EmployeeAdd.cpp : 实现文件
//

#include "stdafx.h"
#include "Callcenter.h"
#include "EmployeeAdd.h"
#include ".\employeeadd.h"


// CEmployeeAdd 对话框

IMPLEMENT_DYNAMIC(CEmployeeAdd, CDialog)
CEmployeeAdd::CEmployeeAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CEmployeeAdd::IDD, pParent)	
	, m_nAge(0)
	, m_strNo(_T(""))
	, m_strName(_T(""))
	, m_strSex(_T(""))
	, m_strPwd1(_T(""))
	, m_strPwd2(_T(""))
{
//	m_strConn="Provider=sqloledb;Data Source=.;Initial Catalog=DB;User Id=sa;Password=;";
	m_strConn="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=.\\DB.mdb";
}

CEmployeeAdd::~CEmployeeAdd()
{
	
}

void CEmployeeAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nAge);
	DDX_Text(pDX, IDC_EDIT_NO, m_strNo);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_CBString(pDX, IDC_COMBO_SEX, m_strSex);
	DDX_Text(pDX, IDC_EDIT_PWD1, m_strPwd1);
	DDX_Text(pDX, IDC_EDIT_PWD2, m_strPwd2);
}


BEGIN_MESSAGE_MAP(CEmployeeAdd, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDEMP, OnBnClickedButtonAddemp)
	ON_BN_CLICKED(IDC_CANCEL1, OnBnClickedCancel1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CEmployeeAdd 消息处理程序

BOOL CEmployeeAdd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_AGE);
	pSpin->SetRange(0,100);
	pSpin->SetPos(m_nAge);

	//
	pConn=mySql.openConnection(_com_util::ConvertStringToBSTR(m_strConn),"","",adOpenUnspecified);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CEmployeeAdd::OnBnClickedButtonAddemp()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_strNo.Trim().IsEmpty())
	{
		AfxMessageBox(_T("请输入用户代码。"));
		return;
	}
	else if(m_strNo.Trim().GetLength() !=4)
	{
		AfxMessageBox(_T("用户代码长度必须为4。"));
		return;
	}
		
	for( int j = 0; j<m_strNo.Trim().GetLength(); j++ )
	{
		if(m_strNo.GetAt(j) > '9' || m_strNo.GetAt(j) < '0' )
		{
			AfxMessageBox(_T("用户代码必须为数字。"));
			return;
		}
	}	

	if(m_strName.Trim().IsEmpty())
	{
		AfxMessageBox(_T("请输入用户姓名。"));
		return;
	}
	else if(m_strName.GetLength() > 10 )
	{
		AfxMessageBox(_T("用户姓名长度不可大于10。"));
		return;
	}

	if(m_strSex.IsEmpty())
	{
		AfxMessageBox(_T("请选择性别。"));
		return;
	}
	else if(m_nAge <= 0 || m_nAge > 100)
	{
		AfxMessageBox(_T("年龄无效。"));
		return;
	}

	if(m_strPwd1.Trim().IsEmpty())
	{
		AfxMessageBox(_T("请输入密码。"));
		return;
	}
	else if(m_strPwd2.Trim().IsEmpty())
	{
		AfxMessageBox(_T("请输入确认密码。"));
		return;
	}
	else if(m_strPwd1.Trim() != m_strPwd2.Trim())
	{
		AfxMessageBox(_T("密码和确认密码不相符。"));
		return;
	}
	
	for( int j = 0; j<m_strPwd1.Trim().GetLength(); j++ )
	{
		if(m_strPwd1.GetAt(j) > '9' || m_strPwd1.GetAt(j) < '0' )
		{
			AfxMessageBox(_T("密码必须为数字。"));
			return;
		}
	}

	if(m_strPwd1.GetLength() < 6 || m_strPwd1.GetLength() >10 )
	{
		AfxMessageBox(_T("密码长度必须在6—10之间。"));
		return;
	}
	
	//
	if( mySql.findUserId(m_strNo,pConn) )
	{
		AfxMessageBox(_T("此用户代码已存在。"));
		return;
	}

	_variant_t RecordsAffected=0;
	CString strSqlAdd="";
	strSqlAdd.Format(_T("insert into UserDetails(UserId,Pwd,Name,Sex,Age) values('%s','%s','%s','%s','%d');"),m_strNo.Trim(),m_strPwd1.Trim(),m_strName.Trim(),m_strSex,m_nAge);
	try
	{
		pConn->Execute(strSqlAdd.AllocSysString(),&RecordsAffected,adCmdText);
		AfxMessageBox(_T("添加成功。"));
		this->OnOK();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	//
	pList2->DeleteAllItems();
	_RecordsetPtr pSet;
	try
	{
		pSet = mySql.getUserDetails(pConn);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	pSet->MoveFirst();
	int i=0;
	CString iStr;
	while(!pSet->EndOfFile)
	{
	//	_bstr_t TheValue1=m_pRecordset->Fields->GetItem("BIG_NAME")->Value;
		iStr.Format(_T("%d"),i);
		pList2->InsertItem(i,iStr);
		pList2->SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("UserId"))));
		pList2->SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("Name"))));
		i++;
		pSet->MoveNext();
	}
	pSet->Close();
	pSet=NULL;

}

void CEmployeeAdd::OnBnClickedCancel1()
{
	// TODO: 在此添加控件通知处理程序代码
	this->OnCancel();
}

void CEmployeeAdd::ResetControl()
{
	m_strNo = "";
	m_strName = "";
	m_strSex= "男";
	m_nAge = 0;
	m_strPwd1 = "";
	m_strPwd2 = "";
	UpdateData(FALSE);
}

void CEmployeeAdd::setList(CListCtrl *listctrl)
{
	pList2 = listctrl;
}

HBRUSH CEmployeeAdd::OnCtlColor(CDC* pDC,CWnd* pWnd,UINT nCtlColor)
{
	int id = pWnd->GetDlgCtrlID();
	if(  id == IDC_REDHINT || id == IDC_REDHINT2 || id == IDC_REDHINT3 || id == IDC_REDHINT4 || id == IDC_REDHINT5 || id == IDC_REDHINT6 )
	{
		m_hRedBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,0,0));	
		return m_hRedBrush;
	}
	
	return CDialog::OnCtlColor(pDC,pWnd,nCtlColor);
}
void CEmployeeAdd::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
//	OnBnClickedButtonAddemp();

	CDialog::OnOK();
}
