#pragma once

#import "msxml3.dll"

class MyXML
{
public:
	MyXML(void);
public:
	~MyXML(void);

private:
	LPTSTR xmlFilePath;

public:
	BOOL findVoiceNameByNodeId(int NodeId,LPTSTR VoiceNode,LPTSTR &VoicePath);
	int getWaitTimeOrReplayNumByNodeId(int NodeId,LPTSTR NodeName);
	CString getSQL(int NodeId);
	BOOL findKeyByCode(int NodeId,char code,int &nextNodeId);
	BOOL findKey0ByCode(int NodeId,char code,int &nextNodeId);
	int getKeyTypeByCode(int NodeId,char code,int &nextNodeId);	//取得现金结算节点的Key子节点的Type属性值
	int getKey0TypeByCode(int NodeId,char code,int &nextNodeId);	//取得现金结算节点的Key子节点的Type属性值
	int getTypeByNodeId(int NodeId);
	BOOL getChannelNoByPhoneNum(int NodeId,int phoneNum,int &channelNo);
	BOOL getPhoneNumByChannelNo(int NodeId,int &phoneNum,int channelNo);
	BOOL getStateByChannelNo(int NodeId,int channelNo,int &state);//取得

	//对日志文件的读写
	BOOL createLogFile(CTime ct);
	BOOL createLogItem(CString strId,CString strType,int channelNo,CString strStartTime,CString strEndTime,CString strDetails);
	BOOL insertTextToLogItem(CString strId,CString strText,CString nodeName,CTime ct = CTime::GetCurrentTime());
	
	BOOL showLogByDay(CTime ct,CListCtrl *pList);
	BOOL showLogByDistance(CTime ct1,CTime ct2,CListCtrl *pList);
	//根据Content获取voice_path，主要用于拼接大写金额语音
	BOOL findVoiceNameByContent(int NodeId,CString content,LPTSTR &VoicePath);

};
