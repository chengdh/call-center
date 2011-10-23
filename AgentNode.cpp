#include "StdAfx.h"
#include "AgentNode.h"
#include "Channel.h"
#include "CTIServer.h"

AgentNode::AgentNode(void)
{
	state = CH_AGENT;
	find = FALSE;	
	channelNo = -1;	
	playCount = 0;
	timeElapse = 0;
	flag1 = FALSE;
}

AgentNode::~AgentNode(void)
{
}

int AgentNode::startPos = 0;

void AgentNode::Run(int LineNo,Channel &channel)
{	
	if( !flag1 )
	{
		flag1 = TRUE;
		CTime tm;
		CString strTime;
		CString strDetails;
		tm=CTime::GetCurrentTime();
		strTime = tm.Format(_T("%Y-%m-%d %X"));
		strDetails = strTime + CString(_T("ת�˹�����; "));
		myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);
	}
	yzDoWork(LineNo,channel);	
}

void AgentNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR VoicePath1 = "";
	LPTSTR ErrorVoice = "";

	CString strState = "";
	CString tmpState = "";

	int tempCTL;   //Store the temporary data
	
	if( state == CH_AGENT )
	{
		waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime1"));
		waitTime2 = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime2"));
		replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
		defaultChannel = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("DefaultChannel"));
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

			TRACE0("\n|$");
			switch(state)
			{
			case CH_AGENT:	
			case CH_CHECKSIG:
				FeedPower( channel.ConnectToLine );
				StartPlaySignal( LineNo,SIG_STOP);
				ResetLine(LineNo,channel);
				ResetLine(channel.ConnectToLine ,*toChannel);
				return;
			case CH_CONNECTED:
				ClearLink( LineNo,channel.ConnectToLine );	
				StopPlayFile(LineNo);
				StartPlaySignal( channel.ConnectToLine ,SIG_BUSY1 );
				if(toChannel->Connected && channelNo != -1)
					pList1->SetItemText(channelNo,2,_T("�Ͽ�"));
				ResetLine(LineNo,channel);	
				return;	
			case CH_WAITMUSIC:
				StopIndexPlayFile(LineNo);
				StartPlaySignal( channel.ConnectToLine ,SIG_BUSY1 );
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
	default:
		break;	
	}//end switch

	switch(state)
	{
	case CH_AGENT:		
		channel.nTimeElapse = 0;
	
		strState = _T("�˹�����");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		for( int i=0; i < channel.TotalLine; i++ )
		{
			if( startPos >= channel.TotalLine )
				startPos = 0;		
			toChannel = node_cs->getChannel( startPos );
			if( toChannel->nType == CHTYPE_USER && toChannel->UsableFlag == TRUE && toChannel->IsLogin )
			{
				FeedRealRing( startPos );
				StartPlaySignal( LineNo,SIG_RINGBACK );
				channel.ConnectToLine = startPos;
				toChannel->ConnectToLine = LineNo;
				state = CH_CHECKSIG;
				timeElapse = 0;
				toChannel->UsableFlag = FALSE;
				find = TRUE;
				channelNo = startPos;
				startPos++;			
				break;	
			}
			startPos++;
		}
		if( find )
		{		
			break;
		}

		TRACE1("\ndefaultChannel=%d\n",defaultChannel);
		toChannel = node_cs->getChannel( defaultChannel );
		if( toChannel->nType == CHTYPE_USER && toChannel->UsableFlag )
		{
			FeedRealRing( defaultChannel );
			StartPlaySignal( LineNo,SIG_RINGBACK );
			channel.ConnectToLine = defaultChannel;
			toChannel->ConnectToLine = LineNo;
			state = CH_CHECKSIG;
			timeElapse = 0;
			toChannel->UsableFlag = FALSE;
			channelNo = defaultChannel;
			break;
        }		
		
		RsetIndexPlayFile(LineNo);
		if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Busy1"),ErrorVoice) )
		{			
			AddIndexPlayFile(LineNo,ErrorVoice);
		}
		if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WaitMusic"),VoicePath1) )
		{
			AddIndexPlayFile(LineNo,VoicePath1);
		}
		StartIndexPlayFile(LineNo);
		state = CH_WAITMUSIC;
		playCount++;
		break;
	case CH_CHECKSIG:	
		strState = _T("����ź���");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		timeElapse += 100;
		if( timeElapse > waitTime2*1000 )
		{
			FeedPower( channel.ConnectToLine );
			StartPlaySignal( LineNo,SIG_STOP );
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NoListen"),VoicePath1) )
				StartPlayFile(LineNo,VoicePath1,0L);
			
			toChannel->Connected = TRUE;
			state = CH_WAITONHOOK;

			//д��־�����˽�����
			myXml.insertTextToLogItem(channel.strItemId,_T("���˽���; "),_T("Details"),channel.ctStartTime);		
			break;
		}

		if( channelNo != -1 )
		{
			CString strState1 = _T("��������");
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
			//д��־����ϯ����
			CString strDetails = _T("");
			CString strTemp = _T("");
			strTemp.Format(_T("%d"),tempCTL);
			if( toChannel->IsLogin )
			{
				strDetails = CString(_T("��ϯ��Ա")) + (LPCTSTR)toChannel->UserId + CString(_T("��ͨ��")) + strTemp + CString(_T("ժ��; "));
			}
			else
			{
				strDetails = CString(_T("Ĭ��ͨ��")) + strTemp + CString(_T("ժ��; "));
			}
			myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);
		}
		break;
	case CH_CONNECTED:
		strState = _T("��ͨ");
		tmpState = pList1->GetItemText(LineNo,2);
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		tmpState = pList1->GetItemText(channelNo,2);
		if( tmpState != strState )
			pList1->SetItemText(channelNo,2,strState);

		if( !OffHookDetect( channel.ConnectToLine ) )
		{
			TRACE0("\n%^%");
			ClearLink( LineNo,channel.ConnectToLine );
			toChannel->UsableFlag = TRUE;
			InitDtmfBuf( channel.ConnectToLine );
			toChannel->TelLength = 0;
			toChannel->ConnectToLine = -1;
			toChannel->DTMFCode[0] = NULL;
			toChannel->nTimeElapse = 0;
			toChannel->Connected = FALSE;
			ResetLine(LineNo,channel);
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
		}
		break;
	case CH_WAITMUSIC:
		strState = _T("�ȴ�����");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		channel.nTimeElapse += 100;		
		if( channel.nTimeElapse > waitTime*1000 )
		{
			channel.nTimeElapse = 0;
			playCount++;

			if( playCount > replayNum )
			{
				StopIndexPlayFile(LineNo);
				if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Busy2"),ErrorVoice) )
				{			
					StartPlayFile(LineNo,ErrorVoice,0L);	
				}
				state = CH_WAITONHOOK;

				//д��־�����˽�����
				myXml.insertTextToLogItem(channel.strItemId,_T("������ϯæ�����˽���; "),_T("Details"),channel.ctStartTime);
				break;
			}

			StopIndexPlayFile(LineNo);
			RsetIndexPlayFile(LineNo);
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Busy1"),ErrorVoice) )
			{			
				AddIndexPlayFile(LineNo,ErrorVoice);	
			}
			if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WaitMusic"),VoicePath1) )
			{
				AddIndexPlayFile(LineNo,VoicePath1);
			}
			StartIndexPlayFile(LineNo);		
		}
		else
		{
			if( CheckIndexPlayFile(LineNo) )
			{
				StopIndexPlayFile(LineNo);
				RsetIndexPlayFile(LineNo);
				if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WaitMusic"),VoicePath1) )
				{
					AddIndexPlayFile(LineNo,VoicePath1);
				}
				StartIndexPlayFile(LineNo);
			}
		}

		//
		for( int i=0; i < channel.TotalLine; i++ )
		{
			if( startPos >= channel.TotalLine )
				startPos = 0;		
			toChannel = node_cs->getChannel( startPos );
			if( toChannel->nType == CHTYPE_USER && toChannel->UsableFlag == TRUE && toChannel->IsLogin )
			{
				FeedRealRing( startPos );
				StopIndexPlayFile(LineNo);
				StartPlaySignal( LineNo,SIG_RINGBACK );
				channel.ConnectToLine = startPos;
				toChannel->ConnectToLine = LineNo;
				state = CH_CHECKSIG;
				timeElapse = 0;
				toChannel->UsableFlag = FALSE;
				find = TRUE;
				channelNo = startPos;
				startPos++;			
				break;	
			}
			startPos++;
		}
		if( find )
		{		
			break;
		}

		toChannel = node_cs->getChannel( defaultChannel );
		if( toChannel->nType == CHTYPE_USER && toChannel->UsableFlag )
		{
			FeedRealRing( defaultChannel );
			StopIndexPlayFile(LineNo);
			StartPlaySignal( LineNo,SIG_RINGBACK );
			channel.ConnectToLine = defaultChannel;
			toChannel->ConnectToLine = LineNo;
			state = CH_CHECKSIG;
			timeElapse = 0;
			toChannel->UsableFlag = FALSE;
			channelNo = defaultChannel;
			break;
        }		
		break;
	case CH_WAITONHOOK:	
		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			ResetLine(LineNo,channel);
			pList1->SetItemText(LineNo,4,_T(""));
			pList1->SetItemText(LineNo,5,_T(""));
			break;
		}	
	default:
		break;
	}//end switch
}

void AgentNode::ResetLine(int LineNo,Channel &channel)
{
	if(channel.nType==CHTYPE_TRUNK)
	{
		FeedPower(channel.ConnectToLine);
		HangUp(LineNo);
		Sig_ResetCheck(LineNo);	//���æ�����Ļ������Լ��ڲ������������Է��һ���æ���󣬱�����ñ�����
		StartSigCheck(LineNo);	//ĳ·��ʼ�µ��ź�����⡣һ����ժ�����߹һ��󣬵��ñ���������ʼ�µ��ź�����⡣
	}

	if( channel.nType == CHTYPE_USER )
	{
		FeedPower( channel.ConnectToLine);	
		StartPlaySignal( LineNo,SIG_STOP );
		toChannel = node_cs->getChannel( channel.ConnectToLine );
		if( toChannel->nType == CHTYPE_TRUNK )
			HangUp( channel.ConnectToLine );
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
	channel.CallerID[0] = NULL;
	channel.DTMFCode[0] = NULL;
	channel.nTimeElapse = 0;

	state = CH_AGENT;
	find = FALSE;
	channelNo = -1;
	playCount = 0;
	timeElapse = 0;
 
	flag1 = FALSE;

	//д��־
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);	
}
