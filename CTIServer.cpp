#include "StdAfx.h"
#include "CTIServer.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

CTIServer::CTIServer(void)
{
}

CTIServer::~CTIServer(void)
{
}

BOOL CTIServer::InitSystem()
{
	DriverOpenFlag = LoadDRV ( );
	if( DriverOpenFlag )
	{
		AfxMessageBox("Load Driver FAIL");
		return FALSE;
	}
	TotalLine = CheckValidCh();
	if ( EnableCard(TotalLine,1024*8) != (long)0) 
	{
		FreeDRV();
		AfxMessageBox ("Enable Board FAIL");
		return FALSE;
	}

	Sig_Init(0);
	
	Lines = new Channel[TotalLine];
	for(int i=0;i<TotalLine;i++)
	{
		InitDtmfBuf(i);
		Lines[i].UsableFlag=TRUE;
		Lines[i].ConnectToLine=-1;
        Lines[i].nType=CheckChTypeNew(i);
		Lines[i].State = CH_FREE;
		Lines[i].TelLength=0;
		Lines[i].CallerID[0]=NULL;
		Lines[i].DTMFCode[0] =NULL;
		Lines[i].nTimeElapse=0;
		Lines[i].IsLogin = FALSE;
		Lines[i].TotalLine = TotalLine;
		Lines[i].No = i;

		Lines[i].Connected = FALSE;
		Lines[i].UserId[0] = NULL;
		Lines[i].PhoneNum[0] = NULL;
		Lines[i].strItemId = _T("");

		Lines[i].flag1 = FALSE;
		Lines[i].ctStartTime = CTime::GetCurrentTime();

		Lines[i].setCTIServer(this);
	}



	return TRUE;
}

void CTIServer::ExitSystem()
{
	for( int LineNo=0; LineNo<TotalLine; LineNo++ )
	{
		if( Lines[LineNo].nType==CHTYPE_TRUNK )
		{
			FeedPower(Lines[LineNo].ConnectToLine);
			HangUp(LineNo);
			Sig_ResetCheck(LineNo);	//清空忙音检测的缓冲区以及内部计数。当检测对方挂机的忙音后，必须调用本函数
			StartSigCheck(LineNo);	//某路开始新的信号音检测。一般在摘机或者挂机后，调用本函数来开始新的信号音检测。
		}

		if( Lines[LineNo].nType == CHTYPE_USER )
		{
			FeedPower( Lines[LineNo].ConnectToLine);	
			StartPlaySignal( LineNo,SIG_STOP );			
			if( Lines[Lines[LineNo].ConnectToLine].nType == CHTYPE_TRUNK )
				HangUp( Lines[LineNo].ConnectToLine );
		}

		if(Lines[LineNo].ConnectToLine!=-1)
		{
			ClearLink(LineNo,Lines[LineNo].ConnectToLine);
			Lines[LineNo].ConnectToLine = -1;
			Lines[Lines[LineNo].ConnectToLine].ConnectToLine = -1;
		}
	}

	DisableCard();
	FreeDRV();
	
	delete []Lines;	
	_CrtDumpMemoryLeaks(); /////////////////////////////////////////
	
}

Channel* CTIServer::getChannel(int LineNo)
{
//	memset(Lines[LineNo].DTMFCode,0,100);
//	Lines[LineNo].DTMFCode[0] = NULL;
	return &Lines[LineNo];
}

void CTIServer::Run()
{
	PUSH_PLAY();
	FeedSigFunc();
	
	for(int LineNo=0;LineNo<TotalLine;LineNo++)
	{
		Lines[LineNo].Run(LineNo);
	}
}


void CTIServer::SetListCtrls(CListCtrl *plist1, CListCtrl *plist2, CListCtrl *plist3, CListCtrl *plist4)
{	
	for(int LineNo=0;LineNo<TotalLine;LineNo++)
	{	
		Lines[LineNo].SetListCtrls(plist1,plist2,plist3,plist4);
	}
}

BOOL CTIServer::checkLoginByUserId(CString UserId,CString &strPhone,int &lineNo)
{
	for(int LineNo=0;LineNo<TotalLine;LineNo++)
	{
		if( Lines[LineNo].IsLogin )
		{
			if( strcmp( Lines[LineNo].UserId,(LPTSTR)(LPCTSTR)UserId ) == 0 )
			{
				strPhone = (LPCTSTR)(LPTSTR)Lines[LineNo].PhoneNum;
				lineNo = Lines[LineNo].No;
				return TRUE;
			}
		}
	}
	return FALSE;
}