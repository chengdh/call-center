#include "StdAfx.h"
#include "LogoutNode.h"
#include "Channel.h"

LogoutNode::LogoutNode(void)
{
	state = CH_LOGOUT;
}

LogoutNode::~LogoutNode(void)
{ 
}


void LogoutNode::Run(int LineNo,Channel &channel)
{
	yzDoWork(LineNo,channel);
}

void LogoutNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR VoicePath1 = "";	

	CString strState = "";
	CString tmpState = "";
	
	switch(channel.nType)
	{
	case CHTYPE_USER:
		if( !OffHookDetect(LineNo) && channel.UsableFlag != TRUE )
		{
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
			TRACE0("\n|");
			switch(state)
			{
			case CH_LOGOUT:			
			case CH_WAITONHOOK:
				StopPlayFile(LineNo);
				StartPlaySignal(LineNo,SIG_STOP);	
				ResetLine(LineNo,channel);
				return;
			default:
				break;
			}
		}
		break;
	case CHTYPE_TRUNK:
		if( Sig_CheckBusy(LineNo) && channel.UsableFlag != TRUE )
		{
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
			switch(state)
			{
			case CH_LOGOUT:			
			case CH_WAITONHOOK:
				StopPlayFile(LineNo);
				StartPlaySignal(LineNo,SIG_STOP);				
				ResetLine(LineNo,channel);
				return;
			default:
				break;			
			}
		break;
		}
	}//end switch

	switch(state)
	{
	case CH_LOGOUT:	
		channel.nTimeElapse = 0;
		strState = _T("��������");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( channel.IsLogin )
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Logout"),VoicePath1) )
			{
				StartPlayFile(LineNo,VoicePath1,0L);
			}
			//������ϯ��Աע��ʱ��
			CTime tm;
			CString strTime;
			tm=CTime::GetCurrentTime();
			strTime = tm.Format(_T("%Y-%m-%d %X"));
			mySql.updateUserRecentLogoutTime( (LPCTSTR)channel.UserId,strTime,pConn );

			//������ϯ��Ա״̬����
			int itemCount = pList2->GetItemCount();
			for(int i=0; i<itemCount; i++)
			{
				if(pList2->GetItemText(i,1) == (LPCTSTR)channel.UserId)
				{
					pList2->SetItemText(i,3,_T(""));
					pList2->SetItemText(i,4,_T(""));
					pList2->SetItemText(i,6,strTime);
					break;
				}
			}						
			//д��־��ע���ɹ���
			CString strDetails;
			strDetails = strTime + CString(_T("��ϯ")) + (LPCTSTR)channel.UserId + CString(_T("ע���ɹ�;"));
			myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);

			channel.IsLogin = FALSE;
			channel.UserId[0] = NULL;
		}
		else
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NotLogin"),VoicePath1) )
			{
				StartPlayFile(LineNo,VoicePath1,0L);
			}
		}	
		state = CH_WAITONHOOK;
		break;
	case CH_WAITONHOOK:
		strState = _T("�ȴ��һ�");
		tmpState = pList1->GetItemText(LineNo,2);
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			StartPlaySignal(LineNo,SIG_BUSY1);
		}
		break;
	default:
		break;
	}//end switch
}

void LogoutNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_TRUNK )
	{
		
	}
	else if( channel.nType == CHTYPE_USER )
	{		

	}

	InitDtmfBuf(LineNo);
	channel.UsableFlag = TRUE;
	channel.ConnectToLine = -1;
 	channel.State = CH_FREE;
	channel.TelLength = 0;
	channel.DTMFCode[0] = NULL;
	channel.nTimeElapse = 0;	
	state = CH_LOGOUT;

	//д��־
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));	
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
}
