#pragma once


#include "Tc08a32.h"
#include "NewSig.h"

#pragma comment(lib,"Tc08a32.lib")
#pragma comment(lib,"NewSig.lib")


#include "MyXML.h"

#include "Node.h"
#include "MenuNode.h"
#include "ResultNode.h"
#include "LoginNode.h"
#include "LogoutNode.h"
#include "InsideNode.h"
#include "OutsideNode.h"
#include "QueryNode.h"
#include "AgentNode.h"
#include "LeaveWordNode.h"


enum CHANNEL_STATE {
	CH_FREE,	//δ��
	CH_LOGIN,
	CH_LOGOUT,
	CH_INSIDE,
	CH_OUTSIDE,
	CH_AGENT,
	CH_QUERY,
	CH_MENU,
	CH_RESULT,
	CH_LEAVEWORD,
};

class Channel
{
public:
	Channel(void);
public:
	~Channel(void);

public:
	BOOL UsableFlag;	//The flag used to indicate the channel be occupied or not(true:not occupied false:occupied)
	int ConnectToLine;	//Another channel that the channel connected to
	int nType;			//The channel type, CHTYPE_USER:user channel,CHTYPE_TRUNK:trunk channel,CHTYPE_RECORD:record channel
	int State;			 //The channel state
	int TelLength;		//The length of tel that the channel receive
	char CallerID[32];	//���к���
	char DTMFCode[100]; //The DTMF that the channel received, will display on the screen
	int nTimeElapse;	
	int CurrNodeId;		//ͨ������������ǰ�ڵ�ID
	BOOL IsLogin;		//ͨ���Ƿ񱻵�¼
	int TotalLine;		//����������ͨ����
	int No;				//ͨ����

	BOOL Connected;		//
	char UserId[100];	//���ڴ洢��¼��ͨ������ϯ��ԱID
	char PhoneNum[10];	//���ڴ洢��ͨ����Ӧ�ķֻ�����
	CString strItemId;		//���ڴ洢������־ĳһ���ID
	
	BOOL flag1;

	CTime ctStartTime;

public: 

	CListCtrl *pList1;
	CListCtrl *pList2;
	CListCtrl *pList3;
	CListCtrl *pList4;

	MyXML myXml;
	MenuNode menuNode;
	ResultNode resultNode;
	LoginNode loginNode;
	LogoutNode logoutNode;
	InsideNode insideNode;
	OutsideNode outsideNode;
	QueryNode queryNode;
	AgentNode agentNode;
	LeaveWordNode leaveWordNode;
	CTIServer* line_cs;
	
public:
	void Run(int LineNo);
	void setCTIServer(CTIServer* cs);
	void changeChannelState(int LineNo,int type);
	void SetListCtrls(CListCtrl *plist1,CListCtrl *plist2,CListCtrl *plist3,CListCtrl *plist4);

};
