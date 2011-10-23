#include "StdAfx.h"
#include "Node.h"

Node::Node(void)
{	
	m_strConn="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=.\\DB.mdb";
	m_strConn1 = "Provider=MSDAORA.1;Data Source=yanzhao;User Id=lmis;Password=67985500";
	try
	{
		pConn = mySql.openConnection(_com_util::ConvertStringToBSTR(m_strConn),"","",adOpenUnspecified);		
		//pConn1 =mySql.openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);		

	}
	catch(_com_error e)
	{
		AfxMessageBox("1"+e.Description());
	}	
}

Node::~Node(void)
{
}

void Node::Run(int LineNo,Channel &channel)
{

}

char Node::GetCodeAscii(int code)
{
	switch(code)
	{
	case 0:		return 'D';		break;
	case 1:		return '1';		break;
	case 2:		return '2';		break;
	case 3:		return '3';		break;
	case 4:		return '4';		break;
	case 5:		return '5';		break;
	case 6:		return '6';		break;
	case 7:		return '7';		break;
	case 8:		return '8';		break;
	case 9:		return '9';		break;
	case 10:	return '0';		break;
	case 11:	return '*';		break;
	case 12:	return '#';		break;
	case 13:	return 'A';		break;
	case 14:	return 'B';		break;
	case 15:	return 'C';		break;
	default:	return -1;
	}
}

void Node::SetListCtrls(CListCtrl *plist1, CListCtrl *plist2, CListCtrl *plist3, CListCtrl *plist4)
{
	pList1 = plist1;
	pList2 = plist2;
	pList3 = plist3;
	pList4 = plist4;
}

void Node::setCTIServer(CTIServer *cs)
{
	node_cs = cs;
}