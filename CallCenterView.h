// CallCenterView.h : CCallCenterView 类的接口
//


#pragma once

#include "MyDialog1.h"
#include "MyDialog2.h"
#include "MyDialog3.h"
#include "MyDialog4.h"
#include "MyDialog5.h"
#include "afxcmn.h"
#include "MyXML.h"

#include "CTIServer.h"

class CCallCenterView : public CView
{
protected: // 仅从序列化创建
	CCallCenterView();
	DECLARE_DYNCREATE(CCallCenterView)

// 属性
public:
	CCallCenterDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	CTabCtrl m_TabCtrl;
	CMyDialog1 myDlg1;
	CMyDialog2 myDlg2;
	CMyDialog3 myDlg3;
	CMyDialog4 myDlg4;
	CMyDialog5 myDlg5;

	CListCtrl *pList1;
	CListCtrl *pList2;
	CListCtrl *pList3;
	CListCtrl *pList4;
	CListCtrl *pList5;
private:
	NOTIFYICONDATA nid;

public:	
	CTIServer ctiServer;

public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnExit();
	afx_msg void OnChannel();
	afx_msg void OnEmployee();
	afx_msg void OnLog();
	afx_msg void OnDSKYW();
	afx_msg void OnAdvice();
	afx_msg void OnUpdateExit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateChannel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEmployee(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLog(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDskyw(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAdvice(CCmdUI *pCmdUI);

public:
	void ChangeTabCtrlItem(int curSel);
public:
	afx_msg LRESULT  OnShowTray(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTrayAbout();
	afx_msg void OnTrayOpen();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
virtual void OnInitialUpdate(); // called first time after construct
protected:

// 实现 
public:
	virtual ~CCallCenterView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // CallCenterView.cpp 的调试版本
inline CCallCenterDoc* CCallCenterView::GetDocument() const
   { return reinterpret_cast<CCallCenterDoc*>(m_pDocument); }
#endif

