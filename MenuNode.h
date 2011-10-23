#pragma once
#include "node.h"

#include "MyXML.h"


class MenuNode :
	public Node
{
public:
	MenuNode(void);
public:
	~MenuNode(void);

public:
	enum MENU_STATE
	{
		CH_SELECT,
		CH_WAITDIAL,
		CH_GETDIAL,
		CH_ERRORINPUT,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;

	LPTSTR VoicePath1;
	LPTSTR VoicePath2;
	LPTSTR VoicePath3;
	LPTSTR HintVoice;

	int waitTime;
	int replayNum;

	BOOL bFlag1;
	BOOL bFlag2;
	BOOL bFlag3;	
	BOOL bFlag4;	

public:
	void Run(int LineNo,Channel &channel);	
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);

};
