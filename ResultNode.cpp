#include "StdAfx.h"
#include "ResultNode.h"
#include "Channel.h"

ResultNode::ResultNode(void)
{
	state = CH_RESULT;
	playCount = 0;
	bFlag1 = FALSE;
	bFlag2 = FALSE;
}

ResultNode::~ResultNode(void)
{
}

void ResultNode::Run(int LineNo,Channel &channel)
{
	yzDoWork(LineNo,channel);
}

void ResultNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR PlayLimitVoice="";
	LPTSTR ErrorVoice="";

	CString strState = "";
	CString tmpState = "";

	int tempCode;   //Store the temporary data
	
	if( state == CH_RESULT && playCount == 0 )
	{
		waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));
		replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
		bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("MainVoice"),VoicePath1);
		bFlag2 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice"),VoicePath2);
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
			case CH_RESULT:
			case CH_WAITDIAL:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL:
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
		}//end if
		break;
	case CHTYPE_TRUNK:
		if( Sig_CheckBusy(LineNo) && channel.UsableFlag != TRUE )
		{
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
			switch(state)
			{
			case CH_RESULT:
			case CH_WAITDIAL:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL:
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
		break;
		}//end if
	}//end switch

	switch(state)
	{
	case CH_RESULT:	
		channel.nTimeElapse = 0;
		strState = _T("播放语音");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( playCount == 0 )
		{
			RsetIndexPlayFile(LineNo);
			if(	bFlag1 )
			{
				AddIndexPlayFile(LineNo,VoicePath1);
			}
			if( bFlag2 )
			{
				AddIndexPlayFile(LineNo,VoicePath2);
			}
			StartIndexPlayFile(LineNo);
			playCount++;
			state = CH_WAITDIAL;
			break;		
		}
		else
		{
			RsetIndexPlayFile(LineNo);
			if( bFlag2 )
			{				
				AddIndexPlayFile(LineNo,VoicePath2);
				StartIndexPlayFile(LineNo);
				playCount++;
				state = CH_WAITDIAL;
				break;
			}
		}
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
				{
					state = CH_RESULT;					
				}
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
				state = CH_RESULT;
				InitDtmfBuf(LineNo);
				channel.TelLength = 0;
				playCount = 0;
			}
			else
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT;
			}
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
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
			channel.TelLength = 0;
			state = CH_RESULT;
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
			if( channel.nType == CHTYPE_TRUNK )
			{
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				pList1->SetItemText(LineNo,4,_T(""));
				pList1->SetItemText(LineNo,5,_T(""));
			}
			else if( channel.nType == CHTYPE_USER )
			{
				StopPlayFile(LineNo);
				StartPlaySignal(LineNo,SIG_BUSY1);
			}
		}
		break;
	default:
		break;
	}//end switch
}

void ResultNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_TRUNK )
	{
		FeedPower(channel.ConnectToLine);
		HangUp(LineNo);
		Sig_ResetCheck(LineNo);
		StartSigCheck(LineNo);
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
	playCount = 0;
	state = CH_RESULT;
	bFlag1 = FALSE;
	bFlag2 = FALSE;

	//写日志
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
}

