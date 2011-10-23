// MyDialog5.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CallCenter.h"
#include "MyDialog5.h"
#include ".\mydialog5.h"
#include "WavePlayer.h"


// CMyDialog5 �Ի���

IMPLEMENT_DYNAMIC(CMyDialog5, CDialog)
CMyDialog5::CMyDialog5(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDialog5::IDD, pParent)
{
}

CMyDialog5::~CMyDialog5()
{
}

void CMyDialog5::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST5, m_ListCtrl5);
	DDX_Control(pDX, IDC_CHECKDATE, m_ctrlCheckDate);
	DDX_Control(pDX, IDC_STARTDATE, m_ctrlStartDate);
	DDX_Control(pDX, IDC_ENDDATE, m_ctrlEndDate);
	DDX_Control(pDX, IDC_STARTTIME, m_ctrlStartTime);
	DDX_Control(pDX, IDC_ENDTIME, m_ctrlEndTime);
	DDX_Control(pDX, IDC_DELDATE, m_ctrlDelDate);
}


BEGIN_MESSAGE_MAP(CMyDialog5, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CHECK, OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_RADIO_DAY, OnBnClickedRadioDay)
	ON_BN_CLICKED(IDC_RADIO_DISTANCE, OnBnClickedRadioDistance)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST5, OnNMDblclkList5)
END_MESSAGE_MAP()


// CMyDialog5 ��Ϣ�������

BOOL CMyDialog5::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ListCtrl5.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES |LVS_EX_TWOCLICKACTIVATE | LVS_EX_UNDERLINECOLD /*| LVS_EX_INFOTIP | LVS_EX_CHECKBOXES*/ );
	m_ListCtrl5.InsertColumn(0,_T("���"),LVCFMT_CENTER,50);
	m_ListCtrl5.InsertColumn(1,_T("�ļ���"),LVCFMT_CENTER,180);
	m_ListCtrl5.InsertColumn(2,_T("����ʱ��"),LVCFMT_CENTER,160);
	m_ListCtrl5.InsertColumn(3,_T("�ļ���С(�ֽ�)"),LVCFMT_CENTER,120);
	
	CButton *pButton1;
	pButton1 = (CButton*)GetDlgItem(IDC_RADIO_DAY);
	pButton1->SetCheck(TRUE);

	m_ctrlStartDate.EnableWindow(FALSE);
	m_ctrlEndDate.EnableWindow(FALSE);	
	m_ctrlStartTime.EnableWindow(FALSE);
	m_ctrlEndTime.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CMyDialog5::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	return;
	CDialog::OnOK();
}

void CMyDialog5::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	return;
	CDialog::OnCancel();
}

void CMyDialog5::OnBnClickedButtonCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ListCtrl5.DeleteAllItems();
	CButton *pButton1,*pButton2;
	pButton1 = (CButton*)GetDlgItem(IDC_RADIO_DAY);
	pButton2 = (CButton*)GetDlgItem(IDC_RADIO_DISTANCE);
	
	if( pButton1->GetCheck() )
	{
		CTime ct1,creationTime;
		CString strTime, strFile, fileName, strFileSize;
		CFileFind fileFind;
		int i = 0;
		CString iStr;

		m_ctrlCheckDate.GetTime(ct1);	
		strFile = CString(_T(".\\Record\\*.*"));
		BOOL isFinded = (BOOL)fileFind.FindFile(strFile);

		while(isFinded)
		{
			isFinded = (BOOL)fileFind.FindNextFile();
			if( !fileFind.IsDots() && !fileFind.IsDirectory() )
			{
				fileName = fileFind.GetFileName();
				if( fileName.GetAt(0) == 't' )
					continue;
				fileFind.GetCreationTime(creationTime);
				if( ct1.GetYear() == creationTime.GetYear() && ct1.GetMonth() == creationTime.GetMonth() && ct1.GetDay() == creationTime.GetDay() )
				{
					iStr.Format(_T("%d"),i);
					m_ListCtrl5.InsertItem( i,iStr );
					m_ListCtrl5.SetItemText(i,1,fileName);
					strTime = creationTime.Format(_T("%Y-%m-%d %X"));
					m_ListCtrl5.SetItemText(i,2,strTime);
					strFileSize.Format(_T("%I64u"),fileFind.GetLength());
					m_ListCtrl5.SetItemText(i,3,strFileSize);
					i++;
				}
			}
		}	
	}
	else if( pButton2->GetCheck() )
	{
		CTime ct1,ct2,ct3,ct4,creationTime;
		CString strTime, strFile, fileName, strFileSize;
		CString strTempTime1,strTempTime2;
		CFileFind fileFind;
		int i = 0;
		CString iStr;
		m_ctrlStartDate.GetTime(ct1);
		m_ctrlEndDate.GetTime(ct2);
		m_ctrlStartTime.GetTime(ct3);
		m_ctrlEndTime.GetTime(ct4);
		strTempTime1 = ct1.Format(_T("%Y-%m-%d ")) + ct3.Format(_T("%X"));
		strTempTime2 = ct2.Format(_T("%Y-%m-%d ")) + ct4.Format(_T("%X"));
		if( strTempTime1 > strTempTime2 )
		{
			AfxMessageBox(_T("��ֹʱ��������ʼʱ�䣬������ѡ��"));
			return;
		}
		else
		{
			strFile = CString(_T(".\\Record\\*.*"));
			BOOL isFinded = (BOOL)fileFind.FindFile(strFile);
			while(isFinded)
			{
				isFinded = (BOOL)fileFind.FindNextFile();
				if( !fileFind.IsDots() && !fileFind.IsDirectory() )
				{
					fileName = fileFind.GetFileName();
					if( fileName.GetAt(0) == 't' )
						continue;
					fileFind.GetCreationTime(creationTime);		
					strTime = creationTime.Format(_T("%Y-%m-%d %X"));				
					if( strTime >= strTempTime1 && strTime <= strTempTime2 )
					{				
						iStr.Format(_T("%d"),i);
						m_ListCtrl5.InsertItem( i,iStr );
						m_ListCtrl5.SetItemText(i,1,fileName);						
						m_ListCtrl5.SetItemText(i,2,strTime);
						strFileSize.Format(_T("%I64u"),fileFind.GetLength());
						m_ListCtrl5.SetItemText(i,3,strFileSize);
						i++;
					}
				}
			}
		}
	}
}

void CMyDialog5::OnBnClickedButtonDel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTime ct1,creationTime;
	CString strTime,strTempTime, strFile, fileName, strFileSize;
	CFileFind fileFind;
	int i = 0;
	CString iStr;

	m_ctrlCheckDate.GetTime(ct1);	
	strFile = CString(_T(".\\Record\\*.*"));
	BOOL isFinded = (BOOL)fileFind.FindFile(strFile);

	while(isFinded)
	{
		isFinded = (BOOL)fileFind.FindNextFile();
		if( !fileFind.IsDots() && !fileFind.IsDirectory() )
		{
			fileName = fileFind.GetFileName();
			if( fileName.GetAt(0) == 't' )
				continue;
			fileFind.GetCreationTime(creationTime);
			strTime = creationTime.Format(_T("%Y-%m-%d"));
			strTempTime = ct1.Format(_T("%Y-%m-%d"));
			if( strTime <= strTempTime )
			{
				fileName = CString(_T(".\\Record\\")) + fileName;
				DeleteFile(fileName);
			}
		}
	}
	AfxMessageBox(_T("����ɾ����ɡ�"));

	//���½���
	m_ListCtrl5.DeleteAllItems();
	i = 0;
	ct1 = CTime::GetCurrentTime();
	isFinded = (BOOL)fileFind.FindFile(strFile);
	while(isFinded)
	{
		isFinded = (BOOL)fileFind.FindNextFile();
		if( !fileFind.IsDots() && !fileFind.IsDirectory() )
		{
			fileName = fileFind.GetFileName();
			if( fileName.GetAt(0) == 't' )
				continue;
			fileFind.GetCreationTime(creationTime);
			if( ct1.GetYear() == creationTime.GetYear() && ct1.GetMonth() == creationTime.GetMonth() && ct1.GetDay() == creationTime.GetDay() )
			{
				iStr.Format(_T("%d"),i);
				m_ListCtrl5.InsertItem( i,iStr );
				m_ListCtrl5.SetItemText(i,1,fileName);
				strTime = creationTime.Format(_T("%Y-%m-%d %X"));
				m_ListCtrl5.SetItemText(i,2,strTime);
				strFileSize.Format(_T("%I64u"),fileFind.GetLength());
				m_ListCtrl5.SetItemText(i,3,strFileSize);
				i++;
			}
		}
	}
}

void CMyDialog5::OnBnClickedRadioDay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ctrlCheckDate.EnableWindow(TRUE);
	m_ctrlStartDate.EnableWindow(FALSE);
	m_ctrlEndDate.EnableWindow(FALSE);
	m_ctrlStartTime.EnableWindow(FALSE);
	m_ctrlEndTime.EnableWindow(FALSE);
}

void CMyDialog5::OnBnClickedRadioDistance()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ctrlStartDate.EnableWindow(TRUE);
	m_ctrlEndDate.EnableWindow(TRUE);
	m_ctrlStartTime.EnableWindow(TRUE);
	m_ctrlEndTime.EnableWindow(TRUE);
	m_ctrlCheckDate.EnableWindow(FALSE);
}

void CMyDialog5::OnNMDblclkList5(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int nItem = pNMLV->iItem;
	if( nItem >= 0 )
	{
		CWavePlayer wavePlayer;
		wavePlayer.setList(&m_ListCtrl5,nItem);
		wavePlayer.DoModal();
	}

	*pResult = 0;
}

