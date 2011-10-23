#include "StdAfx.h"
#include "InsideNode.h"
#include "Channel.h"
#include "CTIServer.h"

InsideNode::InsideNode(void)
{
	state = CH_INSIDE;
	playCount = 0;
	channelNo = -1;
	bFlag1 = FALSE;
	bFlag2 = FALSE;
}

InsideNode::~InsideNode(void)
{
}

void InsideNode::Run(int LineNo,Channel &channel)
{
	yzDoWork(LineNo,channel);
}

void InsideNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR PlayLimitVoice = "";
	LPTSTR ErrorVoice = "";

	CString strState = "";
	CString tmpState = "";

	int tempCTL, tempCode;   //Store the temporary data
	
	if( state == CH_INSIDE && playCount == 0 )
	{
		waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));
		replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
		bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("MainVoice"),VoicePath1);
		bFlag2 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice"),HintVoice) ;
	}

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
			case CH_INSIDE:
			case CH_WAITDIAL:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL:
			case CH_GETDIAL2:
			case CH_DIALING:
			case CH_CHECKSIG:
				FeedPower( channel.ConnectToLine );
				StartPlaySignal( LineNo,SIG_STOP );
				ResetLine(LineNo,channel);
				ResetLine( channel.ConnectToLine,*toChannel );
				myXml.insertTextToLogItem(channel.strItemId,_T("主叫挂机;"),_T("Details"),channel.ctStartTime );				
				return;
			case CH_CONNECTED:
				ClearLink( LineNo,channel.ConnectToLine );				
				StartPlaySignal( channel.ConnectToLine ,SIG_BUSY1 );
				StartPlaySignal( LineNo,SIG_STOP );
				if(toChannel->Connected && channelNo != -1)
					pList1->SetItemText(channelNo,2,_T("断开"));
				ResetLine(LineNo,channel);	
			//	ResetLine( channel.ConnectToLine,*toChannel );
			//	toChannel->UsableFlag = FALSE;				
				return;
			case CH_ERRORINPUT:
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
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
	default:
		break;	
	}//end switch

	switch(state)
	{
	case CH_INSIDE:	
		channel.nTimeElapse = 0;
		strState = _T("播放语音");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
	
	/*	if( !channel.IsLogin )
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NotLogin"),ErrorVoice) )
			{
				StartPlayFile(LineNo,ErrorVoice,0L);
			}
			playCount++;
			state = CH_WAITONHOOK;
			break;
		}*/
		RsetIndexPlayFile(LineNo);
		if(	bFlag1 )
		{
			AddIndexPlayFile(LineNo,VoicePath1);	
		}			
		if( bFlag2 )
		{
			AddIndexPlayFile(LineNo,HintVoice);
		}					
		StartIndexPlayFile(LineNo);
		playCount++;
		state = CH_WAITDIAL;
		break;		
	case CH_WAITDIAL:
		strState = _T("等待拨号");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckIndexPlayFile(LineNo) )
		{
			StopIndexPlayFile(LineNo);			
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount < replayNum )
					state = CH_INSIDE;
				else
				{
					myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("PlayLimit"),PlayLimitVoice);
					StartPlayFile(LineNo,PlayLimitVoice,0L);
					state = CH_WAITONHOOK;
				}
			}
			channel.nTimeElapse += 100;	
		}
		else if( DtmfHit(LineNo) )
		{
			StopIndexPlayFile(LineNo);
		}
		
		if( DtmfHit(LineNo) )
		{
			state = CH_GETDIAL;
			channel.nTimeElapse = 0;
			playCount = 0;
		}
		break;
	case CH_GETDIAL:
		strState = _T("接受用户拨号");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
			channel.DTMFCode[channel.TelLength+1] = NULL;
			channel.TelLength++;

			int nextNodeId = 0;
			if( myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
			{
				channel.CurrNodeId = nextNodeId;	//////////////////////////////////////////////////
				state = CH_INSIDE;
				InitDtmfBuf(LineNo);
				channel.TelLength = 0;
			}
			else if( GetCodeAscii(tempCode) < '0' || GetCodeAscii(tempCode) > '9' )
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT;
				channel.TelLength = 0;
			}
			else
			{
				state = CH_GETDIAL2;
			}
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
		break;
	case CH_GETDIAL2:
		strState = _T("接受用户拨号");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( channel.TelLength < 3 )
		{
			tempCode=GetDtmfCode(LineNo);
			if(tempCode!=-1)
			{
				if( GetCodeAscii(tempCode) < '0' || GetCodeAscii(tempCode) > '9' )
				{
					myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
					StartPlayFile(LineNo,ErrorVoice,0L);
					state = CH_ERRORINPUT;
					channel.TelLength = 0;
					break;
				}
				channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
				channel.DTMFCode[channel.TelLength+1] = NULL;
				channel.TelLength++;	
			}
		}
		else
		{
			state = CH_DIALING;
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
		break;
	case CH_DIALING:
		{
		strState = _T("正在呼出");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCTL = ::atoi( channel.DTMFCode );	
		//写日志
		CTime tm;
		CString strTime;
		CString strDetails;
		CString strCTL;
		strCTL.Format(_T("%d"),tempCTL);
		tm=CTime::GetCurrentTime();
		strTime = tm.Format(_T("%Y-%m-%d %X"));
		strDetails = strTime + CString(_T("拨分机")) + strCTL + CString(_T(","));
	
		myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime );
		
		if( myXml.getChannelNoByPhoneNum(channel.CurrNodeId,tempCTL,channelNo) )
		{
			toChannel = node_cs->getChannel( channelNo );			
			
			if( toChannel->UsableFlag == TRUE )
			{				
				FeedRealRing( channelNo );
				StartPlaySignal( LineNo,SIG_RINGBACK );
				channel.ConnectToLine = channelNo;
				toChannel->ConnectToLine = LineNo;
				state = CH_CHECKSIG;			
				toChannel->UsableFlag = FALSE;
			}
			else
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Busy"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT;
				channel.TelLength = 0;
				//
				myXml.insertTextToLogItem(channel.strItemId,_T("分机忙;"),_T("Details"),channel.ctStartTime );
				break;
			}
		}
		else
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NoPhoneNum"),ErrorVoice) )
			{
				StartPlayFile(LineNo,ErrorVoice,0L);	
				state = CH_ERRORINPUT;
				channel.TelLength = 0;
				//
				myXml.insertTextToLogItem(channel.strItemId,_T("无此分机;"),_T("Details"),channel.ctStartTime );				
				break;
			}	
		}
		break;
		}
	case CH_CHECKSIG:
		strState = _T("检查信号音");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( channelNo != -1 )
		{
			CString strState1 = _T("正在振铃");
			tmpState = pList1->GetItemText(channelNo,2);
			if( tmpState != strState1 )
				pList1->SetItemText(channelNo,2,strState1);
		}

		tempCTL = channel.ConnectToLine;
		if( OffHookDetect(tempCTL) )
		{			
			FeedPower( tempCTL );
			StartPlaySignal( LineNo,SIG_STOP );
			SetLink( LineNo,tempCTL );
			state = CH_CONNECTED;		
			toChannel->Connected = TRUE;//			
			myXml.insertTextToLogItem(channel.strItemId,_T("分机接听;"),_T("Details"),channel.ctStartTime );			
		}
		break;
	case CH_CONNECTED:
		strState = _T("接通");
		tmpState = pList1->GetItemText(LineNo,2);
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		tmpState = pList1->GetItemText(channelNo,2);
		if( tmpState != strState )
			pList1->SetItemText(channelNo,2,strState);

		if( !OffHookDetect( channel.ConnectToLine ) )
		{
			StartPlaySignal( LineNo,SIG_BUSY1 );
			ClearLink( LineNo,channel.ConnectToLine );
			state = CH_WAITONHOOK;
			toChannel->UsableFlag = TRUE;
			InitDtmfBuf( channel.ConnectToLine );
			toChannel->TelLength = 0;
			toChannel->ConnectToLine = -1;
			toChannel->DTMFCode[0] = NULL;
			toChannel->nTimeElapse = 0;
			toChannel->Connected = FALSE;
		}
		break;
	case CH_ERRORINPUT:
		strState = _T("错误输入");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			channel.DTMFCode[0] = NULL;
			state = CH_INSIDE;
		}
		else
		{
			InitDtmfBuf(LineNo);
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
	default:
		break;
	}//end switch
}


void InsideNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_TRUNK )
	{
	//	FeedPower(channel.ConnectToLine);
		HangUp(LineNo);
		Sig_ResetCheck(LineNo);
		StartSigCheck(LineNo);
	}
	else if( channel.nType == CHTYPE_USER )
	{
		FeedPower( channel.ConnectToLine);	
		StartPlaySignal( LineNo,SIG_STOP );
	}

	if(channel.ConnectToLine!=-1)
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

	playCount = 0;
	channelNo = -1;
	state = CH_INSIDE;
	bFlag1 = FALSE;
	bFlag2 = FALSE;

	//写日志
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));
	
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);	
}
