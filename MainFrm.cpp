// MainFrm.cpp : CMainFrame ���ʵ��
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
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
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
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}

	//
	//m_wndToolBar.SetButtonText(0,_T("�˳�"));
	//m_wndToolBar.SetButtonText(1,_T("ͨ��״̬"));
	//m_wndToolBar.SetButtonText(2,_T("��ϯ��Ա"));
	//m_wndToolBar.SetButtonText(3,_T("ϵͳ��־"));
	//m_wndToolBar.SetButtonText(4,_T("����"));
	///////////////����������/////////////////
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

	// TODO: �������Ҫ��������ͣ������ɾ��������
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸� CREATESTRUCT cs ���޸Ĵ������
	// ��ʽ

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;

	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~FWS_ADDTOTITLE;		

	cs.cx = 750;
	cs.cy = 550;

	SetTitle(_T("��������"));

	return TRUE;
}


// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
	}
}

