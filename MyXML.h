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
	int getKeyTypeByCode(int NodeId,char code,int &nextNodeId);	//ȡ���ֽ����ڵ��Key�ӽڵ��Type����ֵ
	int getKey0TypeByCode(int NodeId,char code,int &nextNodeId);	//ȡ���ֽ����ڵ��Key�ӽڵ��Type����ֵ
	int getTypeByNodeId(int NodeId);
	BOOL getChannelNoByPhoneNum(int NodeId,int phoneNum,int &channelNo);
	BOOL getPhoneNumByChannelNo(int NodeId,int &phoneNum,int channelNo);
	BOOL getStateByChannelNo(int NodeId,int channelNo,int &state);//ȡ��

	//����־�ļ��Ķ�д
	BOOL createLogFile(CTime ct);
	BOOL createLogItem(CString strId,CString strType,int channelNo,CString strStartTime,CString strEndTime,CString strDetails);
	BOOL insertTextToLogItem(CString strId,CString strText,CString nodeName,CTime ct = CTime::GetCurrentTime());
	
	BOOL showLogByDay(CTime ct,CListCtrl *pList);
	BOOL showLogByDistance(CTime ct1,CTime ct2,CListCtrl *pList);
	//����Content��ȡvoice_path����Ҫ����ƴ�Ӵ�д�������
	BOOL findVoiceNameByContent(int NodeId,CString content,LPTSTR &VoicePath);

};
