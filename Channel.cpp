#include "StdAfx.h"
#include "Channel.h"
#include "CTIServer.h"
#include <atltime.h>

Channel::Channel(void)
{
}

Channel::~Channel(void)
{
	
}

void Channel::Run(int LineNo)
{
	int type;
	CString strState = "";
	CString tmpState = "";
	
	if( UsableFlag  == TRUE && flag1 == FALSE )
	{
		strState = _T("空闲");
		tmpState = pList1->GetItemText(LineNo,2);
		CString CallID;
		CallID = pList1->GetItemText(LineNo, 3);
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		if(CallID != "")
		{
			pList1->SetItemText(LineNo,3, "");
		}
		if(RingDetect(LineNo))
		{
			if( nType==CHTYPE_USER )
			{
				CTime tm;
				CString strTime;				
				tm=CTime::GetCurrentTime();
				strTime = tm.Format(_T("%Y-%m-%d %X"));
				
				pList1->SetItemText(LineNo,5,strTime);

				//写日志
				CString strId;
				CString strTime1;
				strTime1 = tm.Format(_T("%Y%m%d%H%M%S"));
				strId.Format(_T("%d"),LineNo);
				strId += strTime1;
				myXml.createLogItem(strId,_T("内线摘机"),LineNo,strTime,_T(""),_T(""));
				strItemId = strId;
				ctStartTime = tm;
				TRACE1("LineNo=%d",LineNo);
				InitDtmfBuf(LineNo);
				CurrNodeId = 1;
				UsableFlag = FALSE;

			}
			else if( nType==CHTYPE_TRUNK )
			{
				TRACE1("LineNo=%d",LineNo);
			
				ResetCallerIDBuffer(LineNo);
				nTimeElapse = 0;
				flag1 = TRUE;
				return;
							
			}
			menuNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId( CurrNodeId );
				changeChannelState(LineNo,type);
			}				
		}			
	}
	else if( UsableFlag  == TRUE && flag1 == TRUE )
	{		
		BOOL bOffHook = FALSE;
		if( nTimeElapse > 2000 && RingDetect(LineNo) )
		{
			bOffHook = TRUE;			
		}	
		else if( nTimeElapse > 3000 )
		{
			nTimeElapse = 0;
			flag1 = FALSE;
		}
		/*else if( nTimeElapse > 7000 )
			bOffHook = TRUE;*/

		if( !bOffHook )
		{
			nTimeElapse += 50;			
		}
		else
		{
			nTimeElapse = 0; 
		}
			
		int a = GetCallerIDStr(LineNo,CallerID);
		CString tempCallerID, subCallerID;
		tempCallerID = CallerID;
		subCallerID = tempCallerID.Right(tempCallerID.GetLength()-8);
		if(a == 3)
		{
			//pList1->SetItemText(LineNo,3,(LPCTSTR)CallerID);
			pList1->SetItemText(LineNo,3, subCallerID);
			bOffHook = TRUE;
			TRACE0("\n33333333\n");
		}
		else if(a == 4)
		{	
			//pList1->SetItemText(LineNo,3,(LPCTSTR)CallerID);
			pList1->SetItemText(LineNo,3, subCallerID);
			bOffHook = TRUE;
			TRACE0("\n444444444\n");
		}		
		
		if( bOffHook )
		{
			CTime tm;
			CString strTime;
			tm=CTime::GetCurrentTime();
			strTime = tm.Format(_T("%Y-%m-%d %X"));
			pList1->SetItemText(LineNo,5,strTime);

			//写日志
			CString strId;	
			CString strTime1;
			strTime1 = tm.Format(_T("%Y%m%d%H%M%S"));
			strId.Format(_T("%d"),LineNo);
			strId += strTime1;
			myXml.createLogItem(strId,_T("外线呼入"),LineNo,strTime,_T(""),_T(""));
			if( a == 3 || a==4 )
			{ 
				CString strDetails = CString(_T("主叫号码：")) + (LPCTSTR)CallerID + CString(_T(";"));				
				myXml.insertTextToLogItem(strId,strDetails,_T("Details"));
			}
			strItemId = strId;
			ctStartTime = tm;

			OffHook(LineNo);
			InitDtmfBuf(LineNo);
			StartSigCheck(LineNo);
			CurrNodeId = 2;
			UsableFlag = FALSE;
			flag1 = FALSE;
		}
		else
		{
			return;
		}
		menuNode.Run(LineNo,*this);
		if( !UsableFlag )
		{
			type = myXml.getTypeByNodeId( CurrNodeId );
			changeChannelState(LineNo,type);
		}
	}
	else
	{
		if( Connected == TRUE )
		{
			if( nType == CHTYPE_USER && !OffHookDetect(LineNo) )
			{
				TRACE0("V");
				StartPlaySignal(LineNo,SIG_STOP);
				UsableFlag = TRUE;
				Connected = FALSE;
			}		
			return;
		}
	
		if( State == CH_MENU )
		{
			TRACE0("M");
			menuNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_LOGIN )
		{
			TRACE0("L");
			loginNode.setCTIServer(line_cs);	
			loginNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_LOGOUT )
		{
			TRACE0("l");
			logoutNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_INSIDE )
		{
			TRACE0("I");
			insideNode.setCTIServer(line_cs);
			insideNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_OUTSIDE )
		{
			TRACE0("O");
			outsideNode.setCTIServer(line_cs);
			outsideNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_QUERY )
		{
			TRACE0("Q");
			queryNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_AGENT )
		{
			TRACE0("A");
			agentNode.setCTIServer(line_cs);
			agentNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_RESULT )
		{
			TRACE0("R");
			resultNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
		else if( State == CH_LEAVEWORD )
		{
			TRACE0("W");
			leaveWordNode.Run(LineNo,*this);
			if( !UsableFlag )
			{
				type = myXml.getTypeByNodeId(CurrNodeId);
				changeChannelState(LineNo,type);
			}
		}
	}
}


void Channel::changeChannelState(int LineNo,int type)
{
	switch(type)
	{
	case 1:
		State = CH_MENU;
		break;
	case 2:
		State = CH_LOGIN;
		break;
	case 3:
		State = CH_LOGOUT;
		break;
	case 4:
		State = CH_INSIDE;
		break;
	case 5:
		State = CH_OUTSIDE;
		break;
	case 6:
		State = CH_QUERY;
		break;
	case 7:	
		State = CH_LEAVEWORD;
		break;
	case 8:
		State = CH_AGENT;
		break;
	case 9:
		State = CH_RESULT;
		break;
	}
}

void Channel::setCTIServer(CTIServer* cs)
{
	line_cs = cs;
}

void Channel::SetListCtrls(CListCtrl *plist1, CListCtrl *plist2, CListCtrl *plist3, CListCtrl *plist4)
{
	pList1 = plist1;
	pList2 = plist2;
	pList3 = plist3;
	pList4 = plist4;
	//
	menuNode.SetListCtrls(pList1,pList2,pList3,pList4);
	loginNode.SetListCtrls(pList1,pList2,pList3,pList4);
	logoutNode.SetListCtrls(pList1,pList2,pList3,pList4);
	insideNode.SetListCtrls(pList1,pList2,pList3,pList4);
	outsideNode.SetListCtrls(pList1,pList2,pList3,pList4);
	queryNode.SetListCtrls(pList1,pList2,pList3,pList4);
	agentNode.SetListCtrls(pList1,pList2,pList3,pList4);
	resultNode.SetListCtrls(pList1,pList2,pList3,pList4);
	leaveWordNode.SetListCtrls(pList1,pList2,pList3,pList4);

}
