#pragma once
#include "node.h"

class LogoutNode :
	public Node
{
public:
	LogoutNode(void);
public:
	~LogoutNode(void);

	enum LOGOUT_STATE
	{
		CH_LOGOUT,
		CH_WAITONHOOK,
	};

	int state;

public:
	void Run(int LineNo,Channel &channel);
	void yzDoWork(int LineNo,Channel &channel);
	void ResetLine(int LineNo,Channel &channel);
};

