#include "StdAfx.h"
#include "LoginNode.h"
#include "Channel.h"
#include "CTIServer.h"

LoginNode::LoginNode(void)
{
	state = CH_LOGIN;
	playCount = 0;
	playCount2 = 0;
	playCount3 = 0;
	memset(inputCode,0,100);
	memset(inputCode2,0,100);	
	bFlag1 = FALSE;
	bFlag2 = FALSE;
} 

LoginNode::~LoginNode(void)
{	
}

void LoginNode::Run(int LineNo,Channel &channel)
{
	yzDoWork(LineNo,channel);
}

void LoginNode::yzDoWork(int LineNo,Channel &channel)
{
	LPTSTR AlreadyLoginPath = "";
	LPTSTR VoicePath2 = "";
	LPTSTR VoicePath3 = "";
	LPTSTR PlayLimitVoice = "";
	LPTSTR ErrorVoice = "";

	CString strState = "";
	CString tmpState = "";

	int tempCode;   //Store the temporary data
	
	if( state == CH_LOGIN && playCount == 0 )
	{
		waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));
		replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
		bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Input1"),VoicePath1);
		bFlag2 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice"),HintVoice1);
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
			case CH_LOGIN:
			case CH_ISLOGIN:
			case CH_WAITDIAL1:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL1:	
			case CH_GETDIAL2:				
			case CH_GETPWD:
			case CH_WAITDIAL3:
				StopPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL3:
			case CH_LOGINSUCCESS:
			case CH_WAITDIAL4:
				StopIndexPlayFile(LineNo);
				ResetLine(LineNo,channel);
				return;
			case CH_GETDIAL4:
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
		break;		
	}//end switch

	switch(state)
	{
	case CH_LOGIN:	
		channel.nTimeElapse = 0;
		strState = _T("播放语音");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		RsetIndexPlayFile(LineNo);
				
		if( channel.IsLogin )
		{
			if( playCount == 0 )
			{
                if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("AlreadyLogin1"),AlreadyLoginPath) )
				{
					AddIndexPlayFile(LineNo,AlreadyLoginPath);
				}
			}
		
			if( bFlag2 )
			{
				AddIndexPlayFile(LineNo,HintVoice1);
			}					
			StartIndexPlayFile(LineNo);
			playCount++;
			state = CH_ISLOGIN;
			break;
		}
		else
		{
			if(	bFlag1 )
			{
				AddIndexPlayFile(LineNo,VoicePath1);
			}
			if( bFlag2 )
			{
				AddIndexPlayFile(LineNo,HintVoice1);
			}					
			StartIndexPlayFile(LineNo);
			playCount++;
			state = CH_WAITDIAL1;
			break;
		}		
		break;		
	case CH_WAITDIAL1:
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
					state = CH_LOGIN;
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
		strState = _T("接受用户代码");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			channel.DTMFCode[0] = GetCodeAscii(tempCode);
			channel.DTMFCode[1] = NULL;
			channel.TelLength++;
				
			int nextNodeId = 0;
			if( myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
			{
				channel.CurrNodeId = nextNodeId;
				state = CH_LOGIN;
				InitDtmfBuf(LineNo);
				channel.TelLength = 0;						
			}
			else if( GetCodeAscii(tempCode) == '#' )
			{			
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NoUser"),ErrorVoice);
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
		strState = _T("接受用户代码");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		tempCode = GetDtmfCode(LineNo);
		if(tempCode != -1)
		{
			if( GetCodeAscii(tempCode) == '#' )
			{
				strcpy(inputCode,channel.DTMFCode);	
		
				TRACE2("inputCode=%s, dtmfCode=%s ",inputCode,channel.DTMFCode);
				if( mySql.findUserId((LPCTSTR)inputCode,pConn) )
				{
					state = CH_GETPWD;							
				}
				else
				{
					myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("NoUser"),ErrorVoice);
					StartPlayFile(LineNo,ErrorVoice,0L);
					state = CH_ERRORINPUT;
					channel.TelLength = 0;
					memset(inputCode,0,100);
				}
			}
			else
			{
				channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
				channel.DTMFCode[channel.TelLength+1] = NULL;
				channel.TelLength++;				
			}
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
		break;
	case CH_GETPWD:	
		channel.nTimeElapse = 0;
		channel.TelLength = 0;
		strState = _T("播放语音");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Input2"),VoicePath2) )
		{
			StartPlayFile(LineNo,VoicePath2,0L);
		}			
		playCount2++;
		state = CH_WAITDIAL3;
		break;
	case CH_WAITDIAL3:
		strState = _T("等待拨号");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount2 < replayNum )
					state = CH_GETPWD;
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
			StopPlayFile(LineNo);
		}
		
		if( DtmfHit(LineNo) )
		{
			state = CH_GETDIAL3;
			channel.nTimeElapse = 0;
			playCount2 = 0;
		}
		break;
	case CH_GETDIAL3:
		strState = _T("接受用户密码");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)_T("***") )
			pList1->SetItemText(LineNo,4,(LPCTSTR)_T("***"));

		tempCode = GetDtmfCode(LineNo);
		if( tempCode != -1 )
		{
			if( GetCodeAscii(tempCode) == '#' )
			{
				strcpy(inputCode2,channel.DTMFCode);	
	
				if( mySql.checkPassword( (LPCTSTR)inputCode, (LPCTSTR)inputCode2, pConn) )
				{		
					BOOL flag = FALSE;
					for(int i=0;i<channel.TotalLine;i++)
					{
						if( i == LineNo )
							continue;
						toChannel = node_cs->getChannel( i );
						if ( toChannel->IsLogin && strcmp(inputCode,toChannel->UserId) == 0 )
						{
							myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("AlreadyLogin2"),ErrorVoice);
							StartPlayFile(LineNo,ErrorVoice,0L);
							state = CH_ERRORINPUT;
							channel.TelLength = 0;
							memset(inputCode,0,100);
							memset(inputCode2,0,100);
							flag = TRUE;
							break;
						}						
					}
					if( flag )
						break;

					channel.IsLogin = TRUE;
					strcpy(channel.UserId,inputCode);

					//更新坐席人员登录时间
					CTime tm;
					CString strTime;
					tm=CTime::GetCurrentTime();
					strTime = tm.Format(_T("%Y-%m-%d %X"));
					mySql.updateUserRecentLoginTime( (LPCTSTR)inputCode,strTime,pConn );

					//更新坐席人员状态界面
					int phoneNum = 0;
					CString str1 = _T("");
					CString str2 = _T("");

					if( myXml.getPhoneNumByChannelNo(channel.CurrNodeId,phoneNum,LineNo) )
					{
						str1.Format(_T("%d"),phoneNum);
						str2.Format(_T("%d"),LineNo);
						int itemCount = pList2->GetItemCount();
						for(int i=0; i<itemCount; i++)
						{
							if(pList2->GetItemText(i,1) == (LPCTSTR)inputCode)
							{
								pList2->SetItemText(i,3,str1);
								pList2->SetItemText(i,4,str2);
								pList2->SetItemText(i,5,strTime);
								strcpy(channel.PhoneNum,(LPTSTR)(LPCTSTR)str1);	//存储此通道对应的分机号。
								break;
							}
						}
					}
					//写日志，登录成功。				
					CString strDetails;
					strDetails = strTime + CString(_T("座席")) + (LPCTSTR)channel.UserId + CString(_T("登录成功;"));				
					myXml.insertTextToLogItem(channel.strItemId,strDetails,_T("Details"),channel.ctStartTime);
					
					state = CH_LOGINSUCCESS;
					break;
				}
				else
				{
					myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WrongPwd"),ErrorVoice);
					StartPlayFile(LineNo,ErrorVoice,0L);
					state = CH_ERRORINPUT;
					channel.TelLength = 0;
					memset(inputCode,0,100);
					memset(inputCode2,0,100);
					break;
				}
			}
			else
			{
				channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
				channel.DTMFCode[channel.TelLength+1] = NULL;
				channel.TelLength++;		
				break;
			}
		}
		break;
	case CH_LOGINSUCCESS:
		strState = _T("登录成功");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);
		
		channel.nTimeElapse = 0;
		channel.TelLength = 0;
		RsetIndexPlayFile(LineNo);
		if( playCount3 == 0)
		{
			if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("LoginSuccess"),VoicePath3) )
			{	
				AddIndexPlayFile(LineNo,VoicePath3);
			}
		}
		if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice"),HintVoice1) )
		{
			AddIndexPlayFile(LineNo,HintVoice1);
		}
		StartIndexPlayFile(LineNo);
		playCount3++;
		state = CH_WAITDIAL4;
		break;
	case CH_WAITDIAL4:
		strState = _T("等待拨号");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckIndexPlayFile(LineNo) )
		{
			StopIndexPlayFile(LineNo);
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount3 < replayNum )
					state = CH_LOGINSUCCESS;
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
			state = CH_GETDIAL4;
			channel.nTimeElapse = 0;			
		}
		break;
	case CH_GETDIAL4:
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
			if(myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId))
			{
				playCount3 = 0;
				channel.CurrNodeId = nextNodeId;	
				state = CH_LOGIN;
				InitDtmfBuf(LineNo);
				channel.TelLength = 0;
				memset(inputCode,0,100);
				memset(inputCode2,0,100);
			}
			else
			{
				myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
				StartPlayFile(LineNo,ErrorVoice,0L);
				state = CH_ERRORINPUT1;
			}
		}
		tmpState = pList1->GetItemText(LineNo,4);	
		if( tmpState != (LPCTSTR)channel.DTMFCode )
			pList1->SetItemText(LineNo,4,channel.DTMFCode);
		break;
	case CH_ISLOGIN:
		strState = _T("重复登录");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckIndexPlayFile(LineNo) )
		{
			StopIndexPlayFile(LineNo);
			if( channel.nTimeElapse > waitTime*1000 )
			{
				if( playCount < replayNum )
					state = CH_LOGIN;
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
			channel.nTimeElapse = 0;
	//		playCount = 0;
			//
			tempCode = GetDtmfCode(LineNo);
			if( tempCode != -1 )
			{
				channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
				channel.DTMFCode[channel.TelLength+1] = NULL;
				channel.TelLength++;

				int nextNodeId = 0;
				if(myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId))
				{
					channel.CurrNodeId = nextNodeId;
					state = CH_LOGIN;
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
				tmpState = pList1->GetItemText(LineNo,4);	
				if( tmpState != (LPCTSTR)channel.DTMFCode )
					pList1->SetItemText(LineNo,4,channel.DTMFCode);
			}
		}
		break;
	case CH_ERRORINPUT:
		strState = _T("错误输入");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			channel.DTMFCode[0] = NULL;
			channel.TelLength = 0;
			state = CH_LOGIN;
		}
		else
		{
			InitDtmfBuf(LineNo);
		}
		break;
	case CH_ERRORINPUT1:
		strState = _T("错误输入");
		tmpState = pList1->GetItemText(LineNo,2);	
		if( tmpState != strState )
			pList1->SetItemText(LineNo,2,strState);

		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			channel.DTMFCode[0] = NULL;
			channel.TelLength = 0;
			state = CH_LOGINSUCCESS;
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

		if( CheckPlayEnd(LineNo) )
		{
			StopPlayFile(LineNo);
			StartPlaySignal(LineNo,SIG_BUSY1);
		}
		break;
	default:
		break;
	}//end switch

}

void LoginNode::ResetLine(int LineNo,Channel &channel)
{
	if( channel.nType == CHTYPE_TRUNK )
	{
		
	}
	else if( channel.nType == CHTYPE_USER )
	{
		
	}

	InitDtmfBuf(LineNo);
	channel.UsableFlag = TRUE;
	channel.ConnectToLine =- 1;
 	channel.State = CH_FREE;
	channel.TelLength = 0;
	channel.DTMFCode[0] = NULL;
	channel.nTimeElapse = 0;
	playCount = 0;
	playCount2 = 0;
	playCount3 = 0;
	state = CH_LOGIN;
	memset(inputCode,0,100);
	memset(inputCode2,0,100);
	bFlag1 = FALSE;
	bFlag2 = FALSE;

	//写日志
	CTime tm;
	CString strTime;
	tm=CTime::GetCurrentTime();
	strTime = tm.Format(_T("%Y-%m-%d %X"));
	myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
	
}

