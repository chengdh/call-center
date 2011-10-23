// MyDialog4.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Callcenter.h"
#include "MyDialog4.h"

// CMyDialog4 �Ի���

IMPLEMENT_DYNAMIC(CMyDialog4, CDialog)
CMyDialog4::CMyDialog4(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDialog4::IDD, pParent)
{
	m_strConn="Provider=MSDAORA.1;Data Source=orc;User Id=lmis;Password=lmis;";
}

CMyDialog4::~CMyDialog4()
{
}

void CMyDialog4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST4, m_ListCtrl4);
	DDX_Control(pDX, IDC_COMBOFREIGHT, m_ComboFreight);
	DDX_Control(pDX, IDC_COMBOSTATE, m_ComboState);
}


BEGIN_MESSAGE_MAP(CMyDialog4, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()


// CMyDialog4 ��Ϣ�������
BOOL CMyDialog4::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	//Ϊ�б��֮������ʽ����ṹ
	m_ListCtrl4.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD /*| LVS_EX_INFOTIP | LVS_EX_CHECKBOXES*/ );
	m_ListCtrl4.InsertColumn(0,_T("���"),LVCFMT_CENTER,50);
	m_ListCtrl4.InsertColumn(1,_T("���䵥���"),LVCFMT_CENTER,120);	
	m_ListCtrl4.InsertColumn(2,_T("���ջ���(Ԫ)"),LVCFMT_CENTER,120);
	m_ListCtrl4.InsertColumn(3,_T("����״̬"),LVCFMT_CENTER,100);
	m_ListCtrl4.InsertColumn(4,_T("�������"),LVCFMT_CENTER,100);
//	m_ListCtrl4.InsertColumn(5,_T("ҵ��ί��ʱ��"),LVCFMT_CENTER,120);
//	m_ListCtrl4.InsertColumn(6,_T("����ʱ��"),LVCFMT_CENTER,120);

	//ָ����Ͽ�ĳ�ʼ��
	m_ComboFreight.SetCurSel(0);
	m_ComboState.SetCurSel(0);		

	try
	{
		pConn = mySql.openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn),"","",adOpenUnspecified);		
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}

	CString type = _T("a");	
	this->displayDetailsByFreightState(type, pConn);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMyDialog4::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString type = _T("0");
	CString state = _T("0");
	int curSel = -1;
	m_ListCtrl4.DeleteAllItems();	
	curSel = m_ComboFreight.GetCurSel();
	if(curSel == 0)
		type = _T("a");
	else if(curSel == 1)
		type = _T("1");
	else if(curSel == 2)
		type = _T("2");
	else if(curSel == 3)
		type = _T("4");
	
	m_ListCtrl4.DeleteAllItems();
	this->displayDetailsByFreightState(type, pConn);	
}

void CMyDialog4::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int curSel = -1;
	CString state = "";
	m_ListCtrl4.DeleteAllItems();	
	curSel = m_ComboState.GetCurSel();
	if(curSel == 0)
		state = _T("a");
	else if(curSel == 1)
		state = _T("0");
	else if(curSel == 2)
		state = _T("1");

	_RecordsetPtr pSet;
	pSet = mySql.getFreightDetailsByState(state, pConn);	
	if(pSet == NULL)
		return;
	
	int i=0;
	CString iStr;
	_variant_t varTemp;
	pSet->MoveFirst();
	while(!pSet->EndOfFile)
	{
		iStr.Format(_T("%d"),i);
		m_ListCtrl4.InsertItem(i,iStr);
		m_ListCtrl4.SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("FREIGHTNO"))));
		
		varTemp = pSet->GetCollect(_variant_t("DOFORMONEY"));
		if( varTemp.vt != VT_NULL )
		{
			m_ListCtrl4.SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		}
						
		varTemp = pSet->GetCollect(_variant_t("REMARK"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 2)
				state = _T("δ���");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 4)
				state = _T("���˻�");
			m_ListCtrl4.SetItemText(i,3,state);
		}
		varTemp = pSet->GetCollect(_variant_t("IFDRAWED"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 0)
				state = _T("δ����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�ѽ���");			
			m_ListCtrl4.SetItemText(i,4,state);
		}	
	
		i++;
		pSet->MoveNext();
	}
	pSet->Close();
	pSet = NULL;
}

void CMyDialog4::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	CString strId = "";
	CString state = "";

	m_ListCtrl4.DeleteAllItems();
	pEdit->GetWindowText(strId);
	if(strId.Trim().GetLength() == 0)
	{
		AfxMessageBox(_T("���������䵥��š�"));
		return;
	}
	CString strSql = _T("select count(*) from FREIGHTLIST where FREIGHTNO=");
	if(!mySql.findFreightId(strId,pConn,strSql))
	{
		AfxMessageBox(_T("���䵥�����Ч��"));
		return;
	}
	
	_RecordsetPtr pSet;
	try
	{
		pSet = mySql.getFreightDetailsByFreightId(strId,pConn);
	}
	catch(_com_error e)
	{
		AfxMessageBox(e.Description());
	}
	if( pSet == NULL )
		return;
	pSet->MoveFirst();
	int i=0;
	CString iStr;
	_variant_t varTemp;
	while(!pSet->EndOfFile)
	{
		iStr.Format(_T("%d"),i);
		m_ListCtrl4.InsertItem(i,iStr);
		m_ListCtrl4.SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("FREIGHTNO"))));
				
		varTemp = pSet->GetCollect(_variant_t("DOFORMONEY"));
		if( varTemp.vt != VT_NULL )
		{
			m_ListCtrl4.SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		}
						
		varTemp = pSet->GetCollect(_variant_t("REMARK"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 2)
				state = _T("δ���");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 4)
				state = _T("���˻�");
			m_ListCtrl4.SetItemText(i,3,state);
		}
		varTemp = pSet->GetCollect(_variant_t("IFDRAWED"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 0)
				state = _T("δ����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�ѽ���");			
			m_ListCtrl4.SetItemText(i,4,state);
		}			
		i++;
		pSet->MoveNext();
	}
	pSet->Close();
	pSet = NULL;
}

void CMyDialog4::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnOK();
}

void CMyDialog4::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	return;
	CDialog::OnCancel();
}

void CMyDialog4::displayDetailsByFreightState(CString type, _ConnectionPtr pConn)
{
	CString state = _T("0");
	_RecordsetPtr pSet;	
	pSet = mySql.getFreightDetailsByType(type, pConn);
	if(pSet == NULL)
		return;

	int i = 0;
	CString iStr;
	_variant_t varTemp;
	pSet->MoveFirst();
	while(!pSet->EndOfFile)
	{
		iStr.Format(_T("%d"),i);
		m_ListCtrl4.InsertItem(i,iStr);
		m_ListCtrl4.SetItemText(i,1,_com_util::ConvertBSTRToString((_bstr_t)pSet->GetCollect(_variant_t("DOCUMENTNO"))));
		
		varTemp = pSet->GetCollect(_variant_t("GOODSFEE"));
		if( varTemp.vt != VT_NULL )
		{
			m_ListCtrl4.SetItemText(i,2,_com_util::ConvertBSTRToString((_bstr_t)varTemp));
		}
		varTemp = pSet->GetCollect(_variant_t("GOODSSTATE"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 0)
				state = _T("δ����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�ѵ���");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 9)
				state = _T("�����");
			m_ListCtrl4.SetItemText(i,3,state);
		}	
		varTemp = pSet->GetCollect(_variant_t("GOODSFEESTATE"));
		if( varTemp.vt != VT_NULL )
		{
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 0)
				state = _T("δ����");
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 1)
				state = _T("�ѵ���");	
			if(atoi(_com_util::ConvertBSTRToString((_bstr_t)varTemp)) == 9)
				state = _T("�ѽ���");
			m_ListCtrl4.SetItemText(i,4,state);
		}		
		i++;
		pSet->MoveNext();
	}
	pSet->Close();
	pSet = NULL;
}

