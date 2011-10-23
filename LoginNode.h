#pragma once
#include "node.h"

//#include "Channel.h"

class LoginNode :
	public Node
{
public:
	LoginNode(void);
public:
	~LoginNode(void);

	enum LOGIN_STATE
	{
		CH_LOGIN,
		CH_WAITDIAL1,
		CH_GETDIAL1,
		CH_GETDIAL2,
		CH_GETPWD,
		CH_WAITDIAL3,
		CH_GETDIAL3,
		CH_LOGINSUCCESS,
		CH_WAITDIAL4,
		CH_GETDIAL4,
		CH_ISLOGIN,
		CH_ERRORINPUT,
		CH_ERRORINPUT1,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;
	int playCount2;
	int playCount3;
	char inputCode[100];
	char inputCode2[100];

	Channel* toChannel;

	int waitTime;
	int replayNum;

	LPTSTR VoicePath1;
	LPTSTR HintVoice1;
	BOOL bFlag1;
	BOOL bFlag2;

	/*CString m_strConn;
	MySql mySql;
	_ConnectionPtr pConn;*/

public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);

};
