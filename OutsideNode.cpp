#include "StdAfx.h"
#include "OutsideNode.h"
#include "Channel.h"
#include "CTIServer.h"

OutsideNode::OutsideNode(void)
{
	state = CH_OUTSIDE;
	find = FALSE;
	channelNo = -1;
	flag1 = FALSE;
}

OutsideNode::~OutsideNode(void)
{
}

void OutsideNode::Run(int LineNo,Channel &channel)
{
	if( !flag1 )
	{
		flag1 = TRUE;
		CTime tm;
		CString strTime;
		CString strDetails;
		tm=CTime::GetCurrentTime();
		strTime = tm.Format(_T("%Y-%m-%d %X"));
		strDetails = strTime + CString(_T("座席")) + (LPCTSTR)channel.UserId + CString(_T("拨外线; "));
		myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);
	}
	yzDoWork(LineNo,channel);
}

void OutsideNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR VoicePath1 = "";
	LPTSTR ErrorVoice = "";

	CString strState = "";
	CString tmpState = "";

	int tempCode;   //Store the temporary data
	
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
			case CH_OUTSIDE:			
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;			
			case CH_CONNECTED:
				ClearLink( LineNo,channel.ConnectToLine );	
				StopPlayFile(LineNo);
				if(channelNo != -1)
					pList1->SetItemText(channelNo,2,_T("断开"));
				ResetLine(LineNo,channel);	
		//		ResetLine( channel.ConnectToLine,*toChannel );
				InitDtmfBuf(channel.ConnectToLine);
				toChannel->UsableFlag = TRUE;
				toChannel->ConnectToLine = -1;
				return;		
			case CH_WAITONHOOK:
				StopPlayFile(LineNo);	
				StartPlaySignal(LineNo,SIG_STOP);			
				ResetLine(LineNo,channel);
				return;
			case CH_WAITONHOOK1:
				StartPlaySignal(LineNo,SIG_STOP);		
				ResetLine(LineNo,channel);
				return;
			default:
				break;
			}
		}
		break;
	case CHTYPE_TRUNK:
	default:
		break;	
	}//end switch

	switch(state)
	{
	case CH_OUTSIDE:	
		strState = _T("拨外线");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		
		if( !channel.IsLogin )
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NotLogin"),ErrorVoice) )
			{
				StartPlayFile(LineNo,ErrorVoice,0L);
			}			
			state = CH_WAITONHOOK;
			break;
		}
		
		for( int i=0; i < channel.TotalLine; i++ )
		{
			toChannel = node_cs->getChannel( i );
			if( toChannel->nType == CHTYPE_TRUNK && toChannel->UsableFlag == TRUE )
			{
				int state1 = 0;
				if( myXml.getStateByChannelNo( channel.CurrNodeId, i, state1) )
				{
					if( state1 == 1 )
					{				
						channel.ConnectToLine = i;
						toChannel->ConnectToLine = LineNo;
						toChannel->UsableFlag = FALSE;
						int result;
						result = SetLink(LineNo, i);	
						OffHook(i);
						StartSigCheck(i);
						channel.TelLength = 0;
						state = CH_CONNECTED;
						find = TRUE;
						channelNo = i;
						strState = _T("接通");
						tmpState = pList1->GetItemText(LineNo,2);
						if( tmpState != strState )
							pList1->SetItemText(LineNo,2,strState);
						tmpState = pList1->GetItemText(channelNo,2);
						if( tmpState != strState )
							pList1->SetItemText(channelNo,2,strState);

						break;										
					}
				}
				else
				{
					if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ConfigError"),ErrorVoice) )
					{
						StartPlayFile(LineNo,ErrorVoice,0L);
						strState = _T("配置文件错误");
						tmpState = pList1->GetItemText(LineNo,2);	
						if( tmpState != strState )
							pList1->SetItemText(LineNo,2,strState);
						state = CH_WAITONHOOK;
						break;
					}
				}
			}
		}
		
		if( find )
			break;
		if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Busy"),ErrorVoice) )
		{
			StartPlayFile(LineNo,ErrorVoice,0L);
			state = CH_WAITONHOOK;
			break;
		}
		break;			
	case CH_CONNECTED:
		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
			channel.DTMFCode[channel.TelLength+1] = NULL;
			channel.TelLength++;
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
		if( Sig_CheckBusy( channel.ConnectToLine ))
		{		
			strState = _T("空闲");
			tmpState = pList1->GetItemText(channel.ConnectToLine,2);
			if( tmpState != strState )
				pList1->SetItemText(channel.ConnectToLine,2,strState);
			strState = _T("断开");
			tmpState = pList1->GetItemText(LineNo,2);
			if( tmpState != strState )
				pList1->SetItemText(LineNo,2,strState);

			StartPlaySignal( LineNo,SIG_BUSY1 );
			ClearLink( LineNo,channel.ConnectToLine );	
			HangUp( channel.ConnectToLine );			
			InitDtmfBuf(channel.ConnectToLine);
			toChannel->UsableFlag = TRUE;
			toChannel->ConnectToLine = -1;

			state = CH_WAITONHOOK1;
		}
		break;
	case CH_WAITONHOOK:
		strState = _T("等待挂机");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			StartPlaySignal(LineNo,SIG_BUSY1);
		}
		break;
	case CH_WAITONHOOK1:
		break;
	default:
		break;
	}//end switch
}

void OutsideNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_USER )
	{
		FeedPower( channel.ConnectToLine);	
		StartPlaySignal( LineNo,SIG_STOP );
		toChannel = node_cs->getChannel( channel.ConnectToLine );
		if( toChannel->nType == CHTYPE_TRUNK )
			HangUp( channel.ConnectToLine );
	}

	if(channel.ConnectToLine != -1)
	{
		ClearLink(LineNo,channel.ConnectToLine);
		channel.ConnectToLine = -1;
		toChannel->ConnectToLine = -1;
	}

	InitDtmfBuf(LineNo);
	channel.UsableFlag = TRUE;
	channel.ConnectToLine = -1;
 	channel.State = CH_FREE;
	channel.TelLength = 0;
	channel.DTMFCode[0] = NULL;
	channel.nTimeElapse = 0;

	state = CH_OUTSIDE;
	find = FALSE;
	channelNo = -1;
	flag1 = FALSE;

	//写日志
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));	
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);	
}
