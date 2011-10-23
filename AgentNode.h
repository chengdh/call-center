#pragma once
#include "node.h"

class AgentNode :
	public Node
{
public:
	AgentNode(void);
public:
	~AgentNode(void);

	enum AGENT_STATE
	{
		CH_AGENT,	
		CH_CHECKSIG,
		CH_CONNECTED,
		CH_WAITMUSIC,
		CH_WAITONHOOK,	
	};

	int state;
	char inputCode[100];
	Channel* toChannel;
	BOOL find;
	int channelNo;
	int playCount;
	int timeElapse;
	
	int waitTime;
	int waitTime2;
	int replayNum;
	int defaultChannel;

	static int startPos;

	BOOL flag1;

public:
	void Run(int LineNo,Channel &channel);	
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);
};

