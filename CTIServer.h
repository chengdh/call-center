#pragma once

#include "Channel.h"




class CTIServer
{
public:
	CTIServer(void);
public:
	~CTIServer(void);


public:
	long DriverOpenFlag;
	int TotalLine;
	Channel* Lines;
	

public:
	BOOL InitSystem();
	void ExitSystem();
	void Run();
	Channel* getChannel(int LineNo);
	void SetListCtrls(CListCtrl *plist1,CListCtrl *plist2,CListCtrl *plist3,CListCtrl *plist4);

	BOOL checkLoginByUserId(CString UserId,CString &strPhone,int &lineNo);
};

