// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Callcenter.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MYTOOLBAR3))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	//
	//m_wndToolBar.SetButtonText(0,_T("退出"));
	//m_wndToolBar.SetButtonText(1,_T("通道状态"));
	//m_wndToolBar.SetButtonText(2,_T("坐席人员"));
	//m_wndToolBar.SetButtonText(3,_T("系统日志"));
	//m_wndToolBar.SetButtonText(4,_T("关于"));
	///////////////调整工具条/////////////////
	//CRect rc(0,0,0,0);
	//CSize sizeMax(0,0);
	//CToolBarCtrl& bar=m_wndToolBar.GetToolBarCtrl();
	//for(int nIndex=bar.GetButtonCount()-1;nIndex>=0;nIndex--)
	//{
	//	bar.GetItemRect(nIndex,rc);

	//	rc.NormalizeRect();
	//	sizeMax.cx=__max(rc.Size().cx,sizeMax.cx);
	//	sizeMax.cy=__max(rc.Size().cy,sizeMax.cy);
	//}
	//m_wndToolBar.SetSizes(sizeMax,CSize(16,15));

	// TODO: 如果不需要工具栏可停靠，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或
	// 样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;

	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~FWS_ADDTOTITLE;		

	cs.cx = 750;
	cs.cy = 550;

	SetTitle(_T("呼叫中心"));

	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
}

