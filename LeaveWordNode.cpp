#include "StdAfx.h"
#include ".\leavewordnode.h"
#include "Channel.h"

#define MAX_RECORD_COUNT 9999

int LeaveWordNode::recordCount = 0;

LeaveWordNode::LeaveWordNode(void)
{
	state = CH_LEAVEWORD;
	playCount = 0;
	playCount2 = 0;
	strState = _T("");
	tempFile = _T("");
	strNo = _T("");
	bFlag1 = FALSE;
	bFlag2 = FALSE;
	bFlag3 = FALSE;
	bEnsure = FALSE;
}

LeaveWordNode::~LeaveWordNode(void)
{
}

void LeaveWordNode::Run(int LineNo,Channel &channel)
{
	yzDoWork(LineNo,channel);	
}

void LeaveWordNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR VoicePath3 = "";
	LPTSTR VoicePath4 = "";
	LPTSTR HintVoice2 = "";
	LPTSTR DigitalVoice = "";
	LPTSTR PlayLimitVoice = "";
	LPTSTR ErrorVoice = "";

	CString strState = "";
	CString tmpState = "";

	int tempCode;   //Store the temporary data
	if( state == CH_LEAVEWORD )
	{
		waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));
		replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
		bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("MainVoice"),VoicePath1);
		bFlag2 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice1"),HintVoice1);
		bFlag3 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Select"),VoicePath2);
		tempFile.Format(_T(".\\Record\\temp%d.wav"),LineNo);
	}
	
	switch(channel.nType)
	{
	case CHTYPE_USER:
		break;
	case CHTYPE_TRUNK:
		if( Sig_CheckBusy(LineNo) && channel.UsableFlag != TRUE )
		{
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
			switch(state)
			{
			case CH_LEAVEWORD:
			case CH_RECORD:
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_RECORD1:
				StopRecordFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_SELECT1:
			case CH_WAITDIAL1:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL1:
			case CH_ENSURE:
				ResetLine(LineNo,channel);
				return;
			case CH_RELISTEN:
			case CH_RELISTEN1:
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_WAITDIAL2:
			case CH_GETDIAL2:	
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_ERRORINPUT:
			case CH_ERRORINPUT1:
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
	}//end switch

	switch(state)
	{
	case CH_LEAVEWORD:
		channel.nTimeElapse = 0;
		strState = _T("¡Ù—‘");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
	
		if(	bFlag1 )
		{
			StartPlayFile(LineNo,VoicePath1,0L);
		}
		state = CH_RECORD;
		break;
	case CH_RECORD:
		strState = _T("¡Ù—‘");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			InitDtmfBuf(LineNo);
			StartRecordFileNew(LineNo,(LPTSTR)(LPCTSTR)tempFile,8000*180,0L);
			state = CH_RECORD1;
		}		
		break;
	case CH_RECORD1:
		strState = _T("¡Ù—‘");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckRecordEnd(LineNo) )
		{
			StopRecordFile(LineNo);
			state = CH_SELECT1;
			break;
		}
		if( DtmfHit(LineNo) )
		{
			tempCode=GetDtmfCode(LineNo);
			if( tempCode != -1 )
			{
				channel.DTMFCode[0] = GetCodeAscii(tempCode);
				channel.DTMFCode[1] = NULL;
				if( GetCodeAscii(tempCode) == '#' )
				{
					StopRecordFile(LineNo);
					InitDtmfBuf(LineNo);
					state = CH_SELECT1;							
				}
				tmpState = pList1->GetItemText(LineNo,4);	
				if( tmpState != (LPCTSTR)channel.DTMFCode )
					pList1->SetItemText(LineNo,4,channel.DTMFCode);				
			}
		}
		break;
	case CH_SELECT1:
		channel.nTimeElapse = 0;
		strState = _T("≤•∑≈”Ô“Ù");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
	
		RsetIndexPlayFile(LineNo);
		if(	bFlag3 )
		{
			AddIndexPlayFile(LineNo,VoicePath2);
		}
		if( bFlag2 )
		{
			AddIndexPlayFile(LineNo,HintVoice1);
		}					
		StartIndexPlayFile(LineNo);	
		playCount++;
		state = CH_WAITDIAL1;
		break;
	case CH_WAITDIAL1:
		strState = _T("µ»¥˝≤¶∫≈");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckIndexPlayFile(LineNo) )
		{
			StopIndexPlayFile(LineNo);
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount < replayNum )
					state = CH_SELECT1;
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
			state = CH_GETDIAL1;
			channel.nTimeElapse = 0;	
			playCount = 0;
		}
		break;	
	case CH_GETDIAL1:
		strState = _T("Ω” ‹”√ªß≤¶∫≈");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			channel.DTMFCode[0] = GetCodeAscii(tempCode);
			channel.DTMFCode[1] = NULL;	
				
			int nextNodeId = 0;
			if( myXml.findKey0ByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
			{
				int type = myXml.getKey0TypeByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId);
				if(  type == 1 )
				{
					state = CH_ENSURE;
					channel.CurrNodeId = nextNodeId;
					bEnsure = TRUE;
				}
				else if( type == 2 )
				{
					state = CH_RELISTEN;
					channel.CurrNodeId = nextNodeId;
				}
				else if( type == 3 )
				{
					state = CH_LEAVEWORD;
					channel.CurrNodeId = nextNodeId;
				}
				else
				{
					state = CH_LEAVEWORD;
					channel.CurrNodeId = nextNodeId;						
					InitDtmfBuf(LineNo);
					channel.TelLength = 0;				
				}		
			}			
			else
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT;
			}
			tmpState = pList1->GetItemText(LineNo,4);	
			if( tmpState != (LPCTSTR)channel.DTMFCode )
				pList1->SetItemText(LineNo,4,channel.DTMFCode);
		}		
		break;			
	case CH_ENSURE:
		channel.nTimeElapse = 0;
		strState = _T("≤•±®¡Ù—‘±‡∫≈");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		
		if( bEnsure )
		{
			strNo = getNewLeaveWordNo();
			bEnsure = FALSE;

			CString strNewFile = CString(_T(".\\Record\\")) + strNo + CString(_T(".wav"));
			MoveFile(tempFile,strNewFile);
			//–¥»’÷æ
			CTime tm;
			CString strTime;
			CString strDetails;
			tm=CTime::GetCurrentTime();
			strTime = tm.Format(_T("%Y-%m-%d %X"));
			strDetails = strTime + CString(_T("øÕªß¡Ù—‘,¡Ù—‘±‡∫≈:")) + strNo + CString(_T(";"));
			myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);
		}

		RsetIndexPlayFile(LineNo);
		if( playCount2 == 0 )
		{
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint1"),VoicePath3) )
			{
				AddIndexPlayFile(LineNo,VoicePath3);
			}
			for( int i = 0; i < strNo.GetLength(); i++ )
			{
				char iChar = strNo.GetAt( i );
				CString strDNode = CString(_T("D")) + CString(iChar);				
				if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,(LPTSTR)(LPCTSTR)strDNode,DigitalVoice) )
				{
					AddIndexPlayFile(LineNo,DigitalVoice);
				}				
			}
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint2"),VoicePath4) )
			{
				AddIndexPlayFile(LineNo,VoicePath4);
			}
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice2"),HintVoice2) )
			{
				AddIndexPlayFile(LineNo,HintVoice2);
			}
		}
		else
		{
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice2"),HintVoice2) )
			{
				AddIndexPlayFile(LineNo,HintVoice2);			
			}
		}
		StartIndexPlayFile(LineNo);
		playCount2++;
		state = CH_WAITDIAL2;
		break;
	case CH_WAITDIAL2:
		strState = _T("µ»¥˝≤¶∫≈");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckIndexPlayFile(LineNo) )
		{
			StopIndexPlayFile(LineNo);
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount2 < replayNum )
					state = CH_ENSURE;
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
			state = CH_GETDIAL2;
			channel.nTimeElapse = 0;				
		}
		break;
	case CH_GETDIAL2:
		strState = _T("Ω” ‹”√ªß≤¶∫≈");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			channel.DTMFCode[0] = GetCodeAscii(tempCode);
			channel.DTMFCode[1] = NULL;
				
			int nextNodeId = 0;
			if( myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
			{
				playCount2 = 0;
				int type = myXml.getKeyTypeByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId);
				if(  type == 4 )
				{
					state = CH_ENSURE;
					channel.CurrNodeId = nextNodeId;
				}				
				else
				{
					state = CH_LEAVEWORD;
					channel.CurrNodeId = nextNodeId;						
					InitDtmfBuf(LineNo);
					channel.TelLength = 0;				
				}		
			}			
			else
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT1;
			}
			tmpState = pList1->GetItemText(LineNo,4);	
			if( tmpState != (LPCTSTR)channel.DTMFCode )
				pList1->SetItemText(LineNo,4,channel.DTMFCode);
		}		
		break;
	case CH_RELISTEN:
		strState = _T("÷ÿÃ˝¡Ù—‘");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		StartPlayFile(LineNo,(LPTSTR)(LPCTSTR)tempFile,0L);
		state = CH_RELISTEN1;
        break;
	case CH_RELISTEN1:
		strState = _T("÷ÿÃ˝¡Ù—‘");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			InitDtmfBuf(LineNo);
			state = CH_SELECT1;
		}
		break;
	case CH_ERRORINPUT:
		strState = _T("¥ÌŒÛ ‰»Î");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			channel.DTMFCode[0] = NULL;
			channel.TelLength = 0;
			state = CH_SELECT1;
		}
		else
		{
			InitDtmfBuf(LineNo);
		}
		break;
	case CH_ERRORINPUT1:
		strState = _T("¥ÌŒÛ ‰»Î");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			channel.DTMFCode[0] = NULL;
			channel.TelLength = 0;
			state = CH_ENSURE;
		}
		else
		{
			InitDtmfBuf(LineNo);
		}
		break;
	case CH_WAITONHOOK:
		strState = _T("µ»¥˝π“ª˙");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(CheckPlayEnd(LineNo))
		{
			StopPlayFile(LineNo);
			ResetLine(LineNo,channel);

			strState = _T("ø’œ–");
			tmpState = pList1->GetItemText(LineNo,2);	
			if( tmpState != strState )
				pList1->SetItemText(LineNo,2,strState);
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
		}
		break;
	default:
		break;
	}//end switch
}

void LeaveWordNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_TRUNK )
	{	
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
	playCount2 = 0;
	state = CH_LEAVEWORD;	
	bFlag1 = FALSE;
	bFlag2 = FALSE;
	bFlag3 = FALSE;
	bEnsure = FALSE;
	
	//–¥»’÷æ
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
}

CString LeaveWordNode::getNewLeaveWordNo()
{
	CString strNo = _T("");
	CTime tm;
	CString strTime;
	tm = CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y%m%d%H%M%S"));
	if( recordCount > MAX_RECORD_COUNT )
		recordCount = 0;

	if( recordCount < 10 )
		strNo.Format(_T("000%d"),++recordCount);
	else if( recordCount < 100 )
		strNo.Format(_T("00%d"),++recordCount);
	else if( recordCount < 1000 )
		strNo.Format(_T("0%d"),++recordCount);
	else
		strNo.Format(_T("%d"),++recordCount);

	strNo = strTime + strNo;

	return strNo;
}
