#pragma once
#include "node.h"
//

class InsideNode :
	public Node
{
public:
	InsideNode(void);
public:
	~InsideNode(void);

	enum INSIDE_STATE
	{
		CH_INSIDE,
		CH_WAITDIAL,
		CH_GETDIAL,
		CH_GETDIAL2,
		CH_DIALING,
		CH_CHECKSIG,
		CH_CONNECTED,
		CH_ERRORINPUT,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;	
	char inputCode[100];
	Channel* toChannel;
	int channelNo;

	LPTSTR VoicePath1;
	LPTSTR HintVoice;
	BOOL bFlag1;
	BOOL bFlag2;

	int waitTime;
	int replayNum;


public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);

};
