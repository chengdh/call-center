// EmployeeModify.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Callcenter.h"
#include "EmployeeModify.h"
#include ".\employeemodify.h"


// CEmployeeModify �Ի���

IMPLEMENT_DYNAMIC(CEmployeeModify, CDialog)
CEmployeeModify::CEmployeeModify(CWnd* pParent /*=NULL*/)
	: CDialog(CEmployeeModify::IDD, pParent)
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

CEmployeeModify::~CEmployeeModify()
{

}

void CEmployeeModify::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_AGE, m_nAge);
	DDX_Text(pDX, IDC_EDIT_NO, m_strNo);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_CBString(pDX, IDC_COMBO_SEX, m_strSex);
	DDX_Text(pDX, IDC_EDIT_PWD1, m_strPwd1);
	DDX_Text(pDX, IDC_EDIT_PWD2, m_strPwd2);
}


BEGIN_MESSAGE_MAP(CEmployeeModify, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDEMP, OnBnClickedButtonAddemp)
	ON_BN_CLICKED(IDC_CANCEL1, OnBnClickedCancel1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CEmployeeModify ��Ϣ�������
BOOL CEmployeeModify::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_AGE);
	pSpin->SetRange(0,100);
	pSpin->SetPos(m_nAge);

	//
	pConn=mySql.openConnection(_com_util::ConvertStringToBSTR(m_strConn),"","",adOpenUnspecified);
	_RecordsetPtr pSet;
	try
	{
		pSet = mySql.getUserDetailsByUserId(m_strNo,pConn);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	if( !pSet->EndOfFile )
	{
		pSet->MoveFirst();
		m_strSex = (LPCTSTR)(_bstr_t)pSet->GetCollect(_variant_t("Sex"));
		m_nAge = atoi((_bstr_t)pSet->GetCollect(_variant_t("Age")));
		m_strPwd1 = m_strPwd2 = (LPCTSTR)(_bstr_t)pSet->GetCollect(_variant_t("Pwd"));
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CEmployeeModify::OnBnClickedButtonAddemp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	
	if(m_strName.Trim().IsEmpty())
	{
		AfxMessageBox(_T("�������û�������"));
		return;
	}
	else if(m_strName.GetLength() > 10 )
	{
		AfxMessageBox(_T("�û��������Ȳ��ɴ���10��"));
		return;
	}

	if(m_strSex.IsEmpty())
	{
		AfxMessageBox(_T("��ѡ���Ա�"));
		return;
	}
	else if(m_nAge <= 0 || m_nAge > 100)
	{
		AfxMessageBox(_T("������Ч��"));
		return;
	}

	if(m_strPwd1.Trim().IsEmpty())
	{
		AfxMessageBox(_T("���������롣"));
		return;
	}
	else if(m_strPwd2.Trim().IsEmpty())
	{
		AfxMessageBox(_T("������ȷ�����롣"));
		return;
	}
	else if(m_strPwd1.Trim() != m_strPwd2.Trim())
	{
		AfxMessageBox(_T("�����ȷ�����벻�����"));
		return;
	}
	
	for( int j = 0; j<m_strPwd1.Trim().GetLength(); j++ )
	{
		if(m_strPwd1.GetAt(j) > '9' || m_strPwd1.GetAt(j) < '0' )
		{
			AfxMessageBox(_T("�������Ϊ���֡�"));
			return;
		}
	}

	if(m_strPwd1.GetLength() < 6 || m_strPwd1.GetLength() >10 )
	{
		AfxMessageBox(_T("���볤�ȱ�����6��10֮�䡣"));
		return;
	}
	//
	
	_variant_t RecordsAffected=0;
	CString strSqlUpdate="";
	strSqlUpdate.Format(_T("update UserDetails set Pwd='%s',Name='%s',Sex='%s',Age='%d' where UserId='%s';"),m_strPwd1.Trim(),m_strName.Trim(),m_strSex,m_nAge,m_strNo);
	try
	{
		pConn->Execute(strSqlUpdate.AllocSysString(),&RecordsAffected,adCmdText);
		AfxMessageBox(_T("�޸ĳɹ���"));
		this->OnOK();
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	//

	_RecordsetPtr pSet;
	try
	{
		pSet = mySql.getUserDetailsByUserId(m_strNo,pConn);
	}
	catch(_com_error e)
	{
		AfxMessageBox("333");
	}
	if( !pSet->EndOfFile )
	{
		pSet->MoveFirst();
		pList2->SetItemText(nItem,2,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("Name"))));
		/*pList2->SetItemText(nItem,5,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("RecentLogin"))));
		pList2->SetItemText(nItem,6,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("RecentLogout"))));
		*/
	}
}

void CEmployeeModify::OnBnClickedCancel1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->OnCancel();
}

void CEmployeeModify::ResetControl()
{
	m_strNo = "";
	m_strName = "";
	m_strSex= "��";
	m_nAge = 0;
	m_strPwd1 = "";
	m_strPwd2 = "";
	UpdateData(FALSE);
}

void CEmployeeModify::setList(CListCtrl *listctrl,int nItem)
{
	pList2 = listctrl;
	this->nItem = nItem;
	m_strNo = pList2->GetItemText(nItem,1);
	m_strName = pList2->GetItemText(nItem,2);

}

HBRUSH CEmployeeModify::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������

	int id = pWnd->GetDlgCtrlID();
	if(  id == IDC_REDHINT || id == IDC_REDHINT2 || id == IDC_REDHINT3 || id == IDC_REDHINT4 || id == IDC_REDHINT5 || id == IDC_REDHINT6 )
	{
		m_hRedBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,0,0));	
		return m_hRedBrush;
	}

	return hbr;
}

void CEmployeeModify::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
//	OnBnClickedButtonAddemp();

	CDialog::OnOK();
}
