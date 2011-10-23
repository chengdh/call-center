// CallCenterView.cpp : CCallCenterView 类的实现
//

#include "stdafx.h"
#include "Callcenter.h"
#include "CallCenterDoc.h"
#include "CallCenterView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_ICONNOTIFY WM_USER+123
// CCallCenterView
 
IMPLEMENT_DYNCREATE(CCallCenterView, CView)

BEGIN_MESSAGE_MAP(CCallCenterView, CView)
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CCallCenterView::OnTcnSelchangeTab1)

	ON_COMMAND(ID_EXIT, &CCallCenterView::OnExit)
	ON_COMMAND(ID_CHANNEL, &CCallCenterView::OnChannel)
	ON_COMMAND(ID_EMPLOYEE, &CCallCenterView::OnEmployee)
	ON_COMMAND(ID_LOG, &CCallCenterView::OnLog)
	ON_COMMAND(ID_DSKYW, &CCallCenterView::OnDSKYW)
	ON_COMMAND(ID_ADVICE, &CCallCenterView::OnAdvice)
	
	ON_UPDATE_COMMAND_UI(ID_EXIT, &CCallCenterView::OnUpdateExit)
	ON_UPDATE_COMMAND_UI(ID_CHANNEL, &CCallCenterView::OnUpdateChannel)
	ON_UPDATE_COMMAND_UI(ID_EMPLOYEE, &CCallCenterView::OnUpdateEmployee)
	ON_UPDATE_COMMAND_UI(ID_LOG, &CCallCenterView::OnUpdateLog)
	ON_UPDATE_COMMAND_UI(ID_DSKYW, &CCallCenterView::OnUpdateDskyw)
	ON_UPDATE_COMMAND_UI(ID_ADVICE, &CCallCenterView::OnUpdateAdvice)

	ON_MESSAGE(WM_ICONNOTIFY,OnShowTray)
	
	ON_COMMAND(ID_TRAY_ABOUT, &CCallCenterView::OnTrayAbout)
	ON_COMMAND(ID_TRAY_OPEN, &CCallCenterView::OnTrayOpen)
	ON_WM_TIMER()
	ON_WM_DESTROY()

	ON_WM_SIZE()

END_MESSAGE_MAP()


// CCallCenterView 构造/析构

CCallCenterView::CCallCenterView()
{
	// TODO: 在此处添加构造代码

}

CCallCenterView::~CCallCenterView()
{
	::Shell_NotifyIcon(NIM_DELETE,&nid);
}

BOOL CCallCenterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	return CView::PreCreateWindow(cs);
}

// CCallCenterView 绘制

void CCallCenterView::OnDraw(CDC* /*pDC*/)
{
	CCallCenterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CCallCenterView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	//	
	CRect rect(0,0,0,0);	
	DWORD	dwStyle = TCS_FORCELABELLEFT | TCS_FOCUSNEVER | WS_CHILD | WS_VISIBLE/*| TCS_BOTTOM*/;
	
	m_TabCtrl.Create(dwStyle,rect,this,IDC_TAB2);

	CFont font1;//
	font1.CreateFont(16,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_MODERN,"Courier");
	m_TabCtrl.SetFont(&font1);
	

	myDlg1.Create(IDD_DIALOG1,&m_TabCtrl);
	myDlg2.Create(IDD_DIALOG2,&m_TabCtrl);
	myDlg3.Create(IDD_DIALOG3,&m_TabCtrl);
	//myDlg4.Create(IDD_DIALOG4,&m_TabCtrl);
	//myDlg5.Create(IDD_DIALOG5,&m_TabCtrl);

	TC_ITEM tcItem;
	CString csName="";
	
	csName=CString(_T("通道状态"));
	tcItem.mask=TCIF_TEXT | TCIF_PARAM;
	tcItem.pszText=(LPTSTR)(LPCTSTR)csName;
	tcItem.lParam=(LPARAM)(&myDlg1);
	m_TabCtrl.InsertItem(0,&tcItem);

	csName=CString(_T("坐席人员"));
	tcItem.mask=TCIF_TEXT | TCIF_PARAM;
	tcItem.pszText=(LPTSTR)(LPCTSTR)csName;
	tcItem.lParam=(LPARAM)(&myDlg2);
	m_TabCtrl.InsertItem(1,&tcItem);

	csName = CString(_T("系统日志"));
	tcItem.mask=TCIF_TEXT | TCIF_PARAM;
	tcItem.pszText=(LPTSTR)(LPCTSTR)csName;
	tcItem.lParam=(LPARAM)(&myDlg3);
	m_TabCtrl.InsertItem(2,&tcItem);

	//csName = CString(_T("代收款业务"));
	//tcItem.mask=TCIF_TEXT | TCIF_PARAM;
	//tcItem.pszText=(LPTSTR)(LPCTSTR)csName;
	//tcItem.lParam=(LPARAM)(&myDlg4);
	//m_TabCtrl.InsertItem(3,&tcItem);

	//csName = CString(_T("投诉及建议"));
	//tcItem.mask=TCIF_TEXT | TCIF_PARAM;
	//tcItem.pszText=(LPTSTR)(LPCTSTR)csName;
	//tcItem.lParam=(LPARAM)(&myDlg5);
	//m_TabCtrl.InsertItem(4,&tcItem);

	GetClientRect(rect);
	m_TabCtrl.SetWindowPos(&wndTop,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_SHOWWINDOW);

	m_TabCtrl.GetClientRect(rect);
	rect.top+=25;
	rect.bottom-=48;
	rect.left+=0;
	rect.right-=5;
	myDlg1.MoveWindow(rect);
	myDlg2.MoveWindow(rect);
	myDlg3.MoveWindow(rect);
	//myDlg4.MoveWindow(rect);
	//myDlg5.MoveWindow(rect);

	myDlg1.ShowWindow(TRUE);
	m_TabCtrl.SetCurSel(0);


	//系统托盘
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = IDI_ICON1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_ICONNOTIFY;
	nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	strcpy(nid.szTip,"双击激活呼叫中心主窗口");
	::Shell_NotifyIcon(NIM_ADD,&nid);

	if(!(ctiServer.InitSystem()))
	{
		::Shell_NotifyIcon(NIM_DELETE,&nid);
		::PostQuitMessage(0);				
	}

	myDlg2.setCTIServer(&ctiServer);
	myDlg3.setCTIServer(&ctiServer);
	
//	初始化控件
	CString iStr;
	pList1 = (CListCtrl *)myDlg1.GetDlgItem(IDC_LIST1);
	pList2 = (CListCtrl *)myDlg2.GetDlgItem(IDC_LIST2);
	pList3 = (CListCtrl *)myDlg3.GetDlgItem(IDC_LIST3);
	//pList4 = (CListCtrl *)myDlg4.GetDlgItem(IDC_LIST4);
	//pList5 = (CListCtrl *)myDlg5.GetDlgItem(IDC_LIST5);

	ctiServer.SetListCtrls(pList1,pList2,pList3,pList4);

	for(int i=0;i<ctiServer.TotalLine;i++)
	{
		iStr.Format(_T("%d"),i);
		pList1->InsertItem(i,iStr);
		switch(ctiServer.Lines[i].nType)
		{
		case CHTYPE_USER:
			pList1->SetItemText(i,1,_T("内线"));
			break;
		case CHTYPE_TRUNK:
			pList1->SetItemText(i,1,_T("外线"));
			break;
		case CHTYPE_RECORD:
			pList1->SetItemText(i,1,_T("录音"));
			break;
		case CHTYPE_EMPTY:
			pList1->SetItemText(i,1,_T("空闲"));
			break;
		default:
			break;
		}
	}

	//创建日志文件
	CTime tm;
	CString strTime;
	CString strFile;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d"));
	strFile = CString(_T(".\\Log"));	
	CFileFind fileFind;
	BOOL isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
       	::CreateDirectory(strFile,0);			//创建日志文件夹
	}
	
	strFile = CString(_T(".\\Log\\"))+strTime+CString(_T(".xml"));
	isFinded = (BOOL)fileFind.FindFile(strFile);
	if( !isFinded )
	{
		CoInitialize(NULL);
		MSXML2::IXMLDOMDocument2Ptr pDom;
		HRESULT hr = pDom.CreateInstance(__uuidof(MSXML2::DOMDocument30));
		if(FAILED(hr))
		{
			AfxMessageBox(_T("无法创建DOMDocument对象,请检查是否安装MS XML parser运行库。"));
			pDom = NULL;
			::Shell_NotifyIcon(NIM_DELETE,&nid);
			::PostQuitMessage(0);	
		}
		IXMLDOMProcessingInstructionPtr pPI = NULL;
		pPI = pDom->createProcessingInstruction("xml","version='1.0' encoding='GB2312'");	//创建XML文件头部
		_variant_t vNullVal;
		vNullVal.vt = VT_NULL;
		MSXML2::IXMLDOMNodePtr pNode = pPI;
		pDom->insertBefore(pNode,vNullVal);

		MSXML2::IXMLDOMElementPtr pElement = NULL;
		pElement = pDom->createElement(_T("Log"));
		pDom->appendChild(pElement);
		pElement->setAttribute(_T("Date"),(LPTSTR)(LPCTSTR)strTime);
		
		pDom->save((LPTSTR)(LPCTSTR)strFile);

		pPI = NULL;
		pNode = NULL;
		pElement = NULL;
		pDom = NULL;
		CoUninitialize();
	}

	SetTimer(1000,100,NULL);
	
}

// CCallCenterView 诊断

#ifdef _DEBUG
void CCallCenterView::AssertValid() const
{
	CView::AssertValid();
}

void CCallCenterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCallCenterDoc* CCallCenterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCallCenterDoc)));
	return (CCallCenterDoc*)m_pDocument;
}
#endif //_DEBUG


// CCallCenterView 消息处理程序

int CCallCenterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	return 0;
}

void CCallCenterView::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int curSel;	
	curSel=m_TabCtrl.GetCurSel();
	ChangeTabCtrlItem(curSel);
	
	*pResult = 0;
}

void CCallCenterView::OnExit()
{
	if(AfxMessageBox(_T("确定退出？"),MB_OKCANCEL)==IDOK)
	{
		ctiServer.ExitSystem();
		::Shell_NotifyIcon(NIM_DELETE,&nid);
		::PostQuitMessage(0);		
		//::SendMessage(WM_CLOSE);
		//	exit(0);	
	}
}


void CCallCenterView::OnChannel()
{
	m_TabCtrl.SetCurSel(0);
	ChangeTabCtrlItem(0);
}

void CCallCenterView::OnEmployee()
{
	m_TabCtrl.SetCurSel(1);
	ChangeTabCtrlItem(1);
}

void CCallCenterView::OnLog()
{
	m_TabCtrl.SetCurSel(2);
	ChangeTabCtrlItem(2);
}

void CCallCenterView::OnDSKYW()
{
	m_TabCtrl.SetCurSel(3);
	ChangeTabCtrlItem(3);
}

void CCallCenterView::OnAdvice()
{
	m_TabCtrl.SetCurSel(4);
	ChangeTabCtrlItem(4);
}

void CCallCenterView::OnUpdateExit(CCmdUI *pCmdUI)
{
	
}

void CCallCenterView::OnUpdateChannel(CCmdUI *pCmdUI)
{	
	pCmdUI->SetCheck(m_TabCtrl.GetCurSel()==0);
}

void CCallCenterView::OnUpdateEmployee(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_TabCtrl.GetCurSel()==1);
}

void CCallCenterView::OnUpdateLog(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_TabCtrl.GetCurSel()==2);
}

void CCallCenterView::OnUpdateDskyw(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_TabCtrl.GetCurSel()==3);
}

void CCallCenterView::OnUpdateAdvice(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_TabCtrl.GetCurSel()==4);
}
//
void CCallCenterView::ChangeTabCtrlItem(int curSel)
{
	if( curSel == 0 )
	{
		myDlg2.ShowWindow(FALSE);
		myDlg3.ShowWindow(FALSE);
		//myDlg4.ShowWindow(FALSE);
		//myDlg5.ShowWindow(FALSE);
		myDlg1.ShowWindow(TRUE);
	}
	else if( curSel == 1 )
	{
		myDlg1.ShowWindow(FALSE);
		myDlg3.ShowWindow(FALSE);
		//myDlg4.ShowWindow(FALSE);
		//myDlg5.ShowWindow(FALSE);
		myDlg2.ShowWindow(TRUE);
	}
	else if( curSel == 2 )
	{
		myDlg1.ShowWindow(FALSE);
		myDlg2.ShowWindow(FALSE);
		//myDlg4.ShowWindow(FALSE);
		//myDlg5.ShowWindow(FALSE);
		myDlg3.ShowWindow(TRUE);
	}
	else if( curSel == 3 )
	{
		myDlg1.ShowWindow(FALSE);
		myDlg2.ShowWindow(FALSE);
		myDlg3.ShowWindow(FALSE);
		//myDlg5.ShowWindow(FALSE);
		//myDlg4.ShowWindow(TRUE);
	}
	else if( curSel == 4 )
	{
		myDlg1.ShowWindow(FALSE);
		myDlg2.ShowWindow(FALSE);
		myDlg3.ShowWindow(FALSE);		
		//myDlg4.ShowWindow(FALSE);
		//myDlg5.ShowWindow(TRUE);
	}
}

//
LRESULT  CCallCenterView::OnShowTray(WPARAM wParam, LPARAM lParam)
{
	if(wParam != nid.uID)
		return 0L;

	CMenu menu,*pSubMenu;
	if(LOWORD(lParam) == WM_LBUTTONDBLCLK)
	{
		AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
		AfxGetApp()->m_pMainWnd->SetForegroundWindow();
	}
	else if(LOWORD(lParam) == WM_RBUTTONDOWN)
	{
		if( !menu.LoadMenu(IDR_TRAYMENU))
			return 0L;
		if( !( pSubMenu = menu.GetSubMenu(0)))
			return 0L;
	
		pSubMenu->SetDefaultItem(0,TRUE);
		CPoint pos;
		GetCursorPos(&pos);
		pSubMenu->TrackPopupMenu(0,pos.x,pos.y,this);

		menu.DestroyMenu();
	}

	return 0L;
}

void CCallCenterView::OnTrayAbout()
{
	// TODO: Add your command handler code here
	//此处，若把托盘菜单“关于”的ID改为ID_APP_ABOUT，则消息无法传到CCallCenterApp.
	((CCallCenterApp*)AfxGetApp())->OnAppAbout();
}

void CCallCenterView::OnTrayOpen()
{
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOWNORMAL);
	AfxGetApp()->m_pMainWnd->SetForegroundWindow();
}

void CCallCenterView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	ctiServer.ExitSystem();
}

void CCallCenterView::OnTimer(UINT_PTR nIDEvent)
{
	ctiServer.Run();
	//
	CView::OnTimer(nIDEvent);
}

void CCallCenterView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CRect rect;      
	GetClientRect(rect);
	if(IsWindow(m_TabCtrl.GetSafeHwnd()))
	{
		m_TabCtrl.SetWindowPos(&wndTop,0,0,rect.right-rect.left,rect.bottom-rect.top,SWP_SHOWWINDOW);

		m_TabCtrl.GetClientRect(rect);
		rect.top += 25;
		rect.bottom -= 10; 
		rect.left += 0;
		rect.right -= 3;
		myDlg1.MoveWindow(rect);
		myDlg2.MoveWindow(rect);
		myDlg3.MoveWindow(rect);
		//myDlg4.MoveWindow(rect);
		//myDlg5.MoveWindow(rect);
	}
}


