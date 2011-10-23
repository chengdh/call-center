#pragma once
#include "node.h"

#include "MyXML.h"

class ResultNode :
	public Node
{
public:
	ResultNode(void);
public:
	~ResultNode(void);

	enum RESULT_STATE
	{
		CH_RESULT,
		CH_WAITDIAL,
		CH_GETDIAL,
		CH_ERRORINPUT,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;	

	int waitTime;
	int replayNum;
	LPTSTR VoicePath1;
	LPTSTR VoicePath2;
	BOOL bFlag1;
	BOOL bFlag2;
	
public:
	void Run(int LineNo,Channel &channel);	
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);

};
