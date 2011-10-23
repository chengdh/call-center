#pragma once

//#include "Channel.h"
#include "MyXML.h"
#include "MySql.h"
//#include "CTIServer.h"

class Channel;
class CTIServer;

class Node
{
public:
	Node(void);
public:
	~Node(void);

public:
	MyXML myXml; 
	MySql mySql;
	CString m_strConn,m_strConn1;
	_ConnectionPtr pConn,pConn1;

	CListCtrl *pList1;
	CListCtrl *pList2;
	CListCtrl *pList3;
	CListCtrl *pList4;

	CTIServer* node_cs;
	void setCTIServer(CTIServer* cs);

public:
	virtual void Run(int LineNo, Channel &channel);
	char GetCodeAscii(int code);	
	void SetListCtrls(CListCtrl *plist1,CListCtrl *plist2,CListCtrl *plist3,CListCtrl *plist4);

};
