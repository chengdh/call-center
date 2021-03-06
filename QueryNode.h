#pragma once
#include "node.h"

#include "MyXML.h"


class QueryNode :
	public Node
{
public:
	QueryNode(void);
public:
	~QueryNode(void);

public:
	enum QUERY_STATE
	{
		CH_QUERY,
		CH_WAITDIAL1,
		CH_GETDIAL1,
		CH_GETDIAL2,
		CH_PLAYRESULT,
		CH_WAITDIAL3,
		CH_GETDIAL3,
		CH_ERRORINPUT,
		CH_ERRORINPUT1,
		CH_WAITONHOOK,
	};

	int state;
	int playCount;
	int playCount2;
	char inputCode[100];
	CString strState;
	CString strMoney;
	CString strMoneyState;
	CString strActPayFee;
	int waitTime;
	int replayNum;
	LPTSTR VoicePath1;
	LPTSTR HintVoice1;
	BOOL bFlag1;
	BOOL bFlag2;

private:
	void set_paid_voice(int LineNo,Channel &channel,CString fee);
	void set_canceled_voice(int LineNo,Channel &channel);
	void set_normal_voice(int LineNo,Channel &channel,CString state,CString rmb_num);
	CString rmb_dx(CString rmb_num);
	void set_fee_voice(int LineNo,Channel &channel,CString fee_str);

public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);
};
