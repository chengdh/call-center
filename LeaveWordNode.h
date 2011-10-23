#pragma once
#include "node.h"

#include "MyXML.h"

class LeaveWordNode :
	public Node
{
public:
	LeaveWordNode(void);
	~LeaveWordNode(void);

public:
	enum LEAVEWORD_STATE
	{
		CH_LEAVEWORD,
		CH_RECORD,
		CH_RECORD1,
		CH_SELECT1,
		CH_WAITDIAL1,
		CH_GETDIAL1,
		CH_ENSURE,
		CH_RELISTEN,
		CH_RELISTEN1,
		CH_WAITDIAL2,
		CH_GETDIAL2,	
		CH_ERRORINPUT,
		CH_ERRORINPUT1,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;
	int playCount2;	
	CString strState;

	int waitTime;
	int replayNum;
	LPTSTR VoicePath1;
	LPTSTR VoicePath2;
	LPTSTR HintVoice1;
	BOOL bFlag1;
	BOOL bFlag2;
	BOOL bFlag3;

	CString strNo;
	BOOL bEnsure;

	CString tempFile;
	static int recordCount;

public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);

private:
	CString getNewLeaveWordNo();

};
