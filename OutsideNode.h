#pragma once
#include "node.h"

class OutsideNode :
	public Node
{
public:
	OutsideNode(void);
public:
	~OutsideNode(void);

	enum OUTSIDE_STATE
	{
		CH_OUTSIDE,		
		CH_CONNECTED,	
		CH_WAITONHOOK,
		CH_WAITONHOOK1,
	};

	int state;
	char inputCode[100];
	Channel* toChannel;
	BOOL find;
	int channelNo;

	BOOL flag1;

public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);
};
