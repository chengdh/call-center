#include "StdAfx.h"
#include "QueryNode.h"
#include "Channel.h"

QueryNode::QueryNode(void)
{
  state = CH_QUERY;
  playCount = 0;
  playCount2 = 0;
  memset(inputCode,0,100);
  strState = "";
  strMoney = "";
  strActPayFee = "";
  strMoneyState = "";
  bFlag1 = FALSE;
  bFlag2 = FALSE;
}

QueryNode::~QueryNode(void)
{
}


void QueryNode::Run(int LineNo,Channel &channel)
{
  yzDoWork(LineNo,channel);
}

void QueryNode::yzDoWork(int LineNo,Channel &channel)
{
  LPTSTR VoicePath0 = "";
  LPTSTR VoicePath1 = "";
  LPTSTR VoicePath2 = "";
  LPTSTR VoicePath3 = "";
  LPTSTR VoicePath4 = "";
  LPTSTR VoicePath5 = "";
  LPTSTR VoicePath6 = "";
  LPTSTR VoicePath7 = "";
  LPTSTR VoicePath8 = "";
  LPTSTR VoicePath10 = "";
  LPTSTR HintVoice2 = "";
  LPTSTR PlayLimitVoice = "";
  LPTSTR ErrorVoice = "";

  CString strState = "";
  CString tmpState = "";

  int tempCode;   //Store the temporary data

  if( state == CH_QUERY && playCount == 0 )
  {
    waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));   //�ȴ�ʱ��
    replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));  //�޲�����
    bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("MainVoice"),VoicePath1);   //���ã���ӭ��������������˾�ͻ���������
	//������һ���밴*�ż����˳���һ�,�����˵��£��������
    bFlag2 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice1"),HintVoice1); 
  }

  switch(channel.nType)
  {
    case CHTYPE_USER:
      break;
    case CHTYPE_TRUNK:
      if( Sig_CheckBusy(LineNo) && channel.UsableFlag != TRUE )
      {
        pList1->SetItemText(LineNo,4,_T(""));
        pList1->SetItemText(LineNo,5,_T(""));
        switch(state)
        {
          case CH_QUERY:
          case CH_WAITDIAL1:
            StopIndexPlayFile(LineNo);
            ResetLine(LineNo,channel);
            return;
          case CH_GETDIAL1:
          case CH_GETDIAL2:
          case CH_PLAYRESULT:
          case CH_WAITDIAL3:
            StopIndexPlayFile(LineNo);
            ResetLine(LineNo,channel);
            return;
          case CH_GETDIAL3:
          case CH_ERRORINPUT:
          case CH_ERRORINPUT1:
            StopPlayFile(LineNo);
            ResetLine(LineNo,channel);
            return;
          case CH_WAITONHOOK:
            StopPlayFile(LineNo);
            StartPlaySignal(LineNo,SIG_STOP);
            ResetLine(LineNo,channel);
            return;
          default:
            break;
        }
        break;
      }
  }//end switch
  //pConn1 = mySql.openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);

  switch(state)
  {
	  //��������
    case CH_QUERY:
      channel.nTimeElapse = 0;
      strState = _T("��������");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      RsetIndexPlayFile(LineNo);

      if(	bFlag1 )
      {
        AddIndexPlayFile(LineNo,VoicePath1);
      }
      if( bFlag2 )
      {
        AddIndexPlayFile(LineNo,HintVoice1);
      }
      StartIndexPlayFile(LineNo);

      playCount++;
      state = CH_WAITDIAL1;
      break;
	//�ȴ��û�����
    case CH_WAITDIAL1:
      strState = _T("�ȴ�����");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if( CheckIndexPlayFile(LineNo) )
      {
        StopIndexPlayFile(LineNo);
		//�ж��Ƿ��ѵ��޲�����
        if( channel.nTimeElapse > waitTime*1000 )
        {
          if( playCount < replayNum )
            state = CH_QUERY;
          else
          {
            myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("PlayLimit"),PlayLimitVoice);
            StartPlayFile(LineNo,PlayLimitVoice,0L);
            state = CH_WAITONHOOK;
          }
        }
        channel.nTimeElapse += 100;
      }
      else if( DtmfHit(LineNo) )
      {
        StopIndexPlayFile(LineNo);
      }

      if( DtmfHit(LineNo) )
      {
        state = CH_GETDIAL1;
        channel.nTimeElapse = 0;
        playCount = 0;
      }
      break;

	//��ȡ�û����� 1��0
    case CH_GETDIAL1:
      strState = _T("�����û�����");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      tempCode = GetDtmfCode(LineNo);
      if( tempCode != -1 )
      {
        channel.DTMFCode[0] = GetCodeAscii(tempCode);
        channel.DTMFCode[1] = NULL;
        channel.TelLength++;

        int nextNodeId = 0;
		//�����û������ж�ִ�нڵ�
        if( myXml.findKey0ByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
        {
          channel.CurrNodeId = nextNodeId;
          state = CH_QUERY;
          InitDtmfBuf(LineNo);
          channel.TelLength = 0;
        }
        else if( GetCodeAscii(tempCode) == '#' )
        {
          myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WrongId"),ErrorVoice);
          StartPlayFile(LineNo,ErrorVoice,0L);
          state = CH_ERRORINPUT;
          channel.TelLength = 0;
        }
        else
        {
          state = CH_GETDIAL2;
        }
      }
      tmpState = pList1->GetItemText(LineNo,4);
      if( tmpState != (LPCTSTR)channel.DTMFCode )
        pList1->SetItemText(LineNo,4,channel.DTMFCode);
      break;
	  
	  //�����˵���Ų�ѯ�˵�
    case CH_GETDIAL2:
      strState = _T("�����˵����");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      tempCode = GetDtmfCode(LineNo);
      if(tempCode != -1)
      {
        if( GetCodeAscii(tempCode) == '#' )
        {
          /*if(channel.TelLength != 8)
            {
            myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WrongLength"),ErrorVoice);
            StartPlayFile(LineNo,ErrorVoice,0L);
            state = CH_ERRORINPUT;
            channel.TelLength = 0;
            break;
            }
            */

          strcpy(inputCode,channel.DTMFCode);
          //
          CString strSql = _T("");
          //�˴��ж��˵��Ƿ����
          strSql = myXml.getSQL(channel.CurrNodeId);
          if( mySql.findFreightId( (LPCTSTR)inputCode,pConn1,strSql) )
          {
            state = CH_PLAYRESULT;
          }
          else
          {
            myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("WrongId"),ErrorVoice);
            StartPlayFile(LineNo,ErrorVoice,0L);
            state = CH_ERRORINPUT;
            channel.TelLength = 0;
            memset(inputCode,0,100);
          }
        }
        else
        {
          channel.DTMFCode[channel.TelLength] = GetCodeAscii(tempCode);
          channel.DTMFCode[channel.TelLength+1] = NULL;
          channel.TelLength++;
        }
      }
      tmpState = pList1->GetItemText(LineNo,4);
      if( tmpState != (LPCTSTR)channel.DTMFCode )
        pList1->SetItemText(LineNo,4,channel.DTMFCode);
      break;

	//���Ų�ѯ���
    case CH_PLAYRESULT:
      channel.nTimeElapse = 0;
      channel.TelLength = 0;
      strState = _T("��������");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState)
        pList1->SetItemText(LineNo,2,strState);

      RsetIndexPlayFile(LineNo);

      strMoney = _T("");
      strState = _T("");
      strState = mySql.getStateFromFreight(inputCode,pConn1);
	  strMoney = mySql.getMoneyFromFreight(inputCode,pConn1);
	  strActPayFee = mySql.getActPayFee(inputCode,pConn1);
      //strMoneyState = mySql.getMoneyStateFromFreight(inputCode, pConn1);
      if( playCount2 == 0 )
      {
	
		    if(strState.Trim()== CString("paid"))
				set_paid_voice(LineNo,channel,strActPayFee);

			else if(strState.Trim() == CString("canceled"))
				set_canceled_voice(LineNo,channel);
			else
				set_normal_voice(LineNo,channel,strState,strMoney);

      }

      if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("up_home"),HintVoice2) )
      {
        AddIndexPlayFile(LineNo,HintVoice2);
      }
      StartIndexPlayFile(LineNo);
      playCount2++;
      state = CH_WAITDIAL3;
      break;
    case CH_WAITDIAL3:
      strState = _T("�ȴ�����");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if( CheckIndexPlayFile(LineNo) )
      {
        StopIndexPlayFile(LineNo);
        if( channel.nTimeElapse > waitTime*1000 )
        {
          if( playCount2 < replayNum )
            state = CH_PLAYRESULT;
          else
          {
            myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("PlayLimit"),PlayLimitVoice);
            StartPlayFile(LineNo,PlayLimitVoice,0L);
            state = CH_WAITONHOOK;
          }
        }
        channel.nTimeElapse += 100;
      }
      else if( DtmfHit(LineNo) )
      {
        StopIndexPlayFile(LineNo);
      }

      if( DtmfHit(LineNo) )
      {
        state = CH_GETDIAL3;
        channel.nTimeElapse = 0;
        //	playCount2 = 0;
      }
      break;
    case CH_GETDIAL3:
      strState = _T("�����û�����");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      tempCode = GetDtmfCode(LineNo);
      if( tempCode != -1 )
      {
        channel.DTMFCode[0] = GetCodeAscii(tempCode);
        channel.DTMFCode[1] = NULL;
        channel.TelLength++;

        int nextNodeId = 0;
        if( myXml.findKeyByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) )
        {
          playCount2 = 0;
          if( myXml.getKeyTypeByCode(channel.CurrNodeId,GetCodeAscii(tempCode),nextNodeId) == 0 )
          {
            state = CH_PLAYRESULT;
            channel.CurrNodeId = nextNodeId;
          }
          else
          {
            state = CH_QUERY;
            InitDtmfBuf(LineNo);
            memset(inputCode,0,100);
            channel.CurrNodeId = nextNodeId;
            channel.TelLength = 0;
          }
        }
        else
        {
          myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("ErrorVoice"),ErrorVoice);
          StartPlayFile(LineNo,ErrorVoice,0L);
          state = CH_ERRORINPUT1;
        }
      }
      tmpState = pList1->GetItemText(LineNo,4);
      if( tmpState != (LPCTSTR)channel.DTMFCode )
        pList1->SetItemText(LineNo,4,channel.DTMFCode);
      break;
    case CH_ERRORINPUT:
      strState = _T("��������");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if(CheckPlayEnd(LineNo))
      {
        StopPlayFile(LineNo);
        channel.DTMFCode[0] = NULL;
        state = CH_QUERY;
      }
      else
      {
        InitDtmfBuf(LineNo);
      }
      break;
    case CH_ERRORINPUT1:
      strState = _T("��������");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if(CheckPlayEnd(LineNo))
      {
        StopPlayFile(LineNo);
        channel.DTMFCode[0] = NULL;
        state = CH_PLAYRESULT;
      }
      else
      {
        InitDtmfBuf(LineNo);
      }
      break;
    case CH_WAITONHOOK:
      strState = _T("�ȴ��һ�");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if(CheckPlayEnd(LineNo))
      {
        StopPlayFile(LineNo);
        ResetLine(LineNo,channel);

        strState = _T("����");
        tmpState = pList1->GetItemText(LineNo,2);
        if( tmpState != strState )
          pList1->SetItemText(LineNo,2,strState);
        pList1->SetItemText(LineNo,4,_T(""));
        pList1->SetItemText(LineNo,5,_T(""));
      }
      break;
    default:
      break;
  }//end switch

}
//���������
//���Ļ�����ת��
//ת�˽� xxx
void QueryNode::set_paid_voice(int LineNo,Channel &channel,CString fee){
	LPTSTR voice_path_wait = "";
	LPTSTR voice_path_paid = "";
	LPTSTR voice_path_goods_fee_is = "";
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("wait"),voice_path_wait) ) //���ݲ�ѯ��,��ȴ�
            AddIndexPlayFile(LineNo,voice_path_wait);
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("paid"),voice_path_paid) ) //���Ļ�����ת��
            AddIndexPlayFile(LineNo,voice_path_paid);
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("goods_fee_is"),voice_path_goods_fee_is) ) //ת�˽��
            AddIndexPlayFile(LineNo,voice_path_goods_fee_is);
	set_fee_voice(LineNo,channel,fee);
	//������
}
//����������
//��Ʊ������
void QueryNode::set_canceled_voice(int LineNo,Channel &channel){
	LPTSTR voice_path_invalidated = "";
	LPTSTR voice_path_wait = "";
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("wait"),voice_path_wait) ) //���ݲ�ѯ��,��ȴ�
            AddIndexPlayFile(LineNo,voice_path_wait);
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("invalidated"),voice_path_invalidated) ) //��Ʊ������
            AddIndexPlayFile(LineNo,voice_path_invalidated);
}
//������������
//���Ļ��� xxxxԪ
//����״̬
void QueryNode::set_normal_voice(int LineNo,Channel &channel,CString state,CString rmb_num){
	LPTSTR voice_path_your_goods_fee = ""; 
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("your_goods_fee"),voice_path_your_goods_fee) ) //���Ļ���
		AddIndexPlayFile(LineNo,voice_path_your_goods_fee);

	//��������
	set_fee_voice(LineNo,channel,rmb_num);
	LPTSTR voice_path_goods_state_is = "";
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("goods_state_is"),voice_path_goods_state_is) ) //����״̬
		AddIndexPlayFile(LineNo,voice_path_goods_state_is);

	LPTSTR voice_path_goods_state = "";
	if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,state.GetBuffer(20),voice_path_goods_state) ) //״̬
		AddIndexPlayFile(LineNo,voice_path_goods_state);
}

//������ת��Ϊ��д����
CString QueryNode::rmb_dx(CString rmb_num){
	CString szChMoney,szNum; 
	int iLen, iNum, iAddZero=0;

	TCHAR* hzUnit[18]={_T("��"),_T("��"),_T("Ԫ"),_T("ʰ"),_T("��"),_T("Ǫ"),_T("��"),_T("ʰ"),_T("��"),_T("Ǫ"),_T("��"),_T("ʰ"),_T("��"),_T("Ǫ"),_T("��"),_T("ʰ"),_T("��"),_T("Ǫ")};
	TCHAR* hzNum[10]={_T("��"),_T("Ҽ"),_T("��"),_T("��"),_T("��"),_T("��"),_T("½"),_T("��"),_T("��"),_T("��")};
 
	//szNum.Format(_T("%18.0f"), rmb_num*100); //�������ܻ���������doubleֻ��15λ��Ч����,����ֻ��13
	szNum = rmb_num;
	szNum.TrimLeft();
	iLen=szNum.GetLength();

	if(iLen>15 || iLen==0)
		return ""; //���ݴ��󷵻�

	for(int i=0;i<iLen;i++){
		iNum=_ttoi((LPCTSTR)szNum.Mid(i,1));
		if(iNum==0)
			iAddZero++;
		else{
			if(iAddZero>0) 
				szChMoney+=_T("��");
			szChMoney+=hzNum[iNum];
			iAddZero=0;
		}   
		if(iNum!=0||iLen-i==3||iLen-i==11||((iLen-i+1)%8==0&&iAddZero<4)) //��λ��Ϊ0||Ԫλ||��λ||��λ
			szChMoney+=hzUnit[iLen-i-1];
	}

	if(szNum.Right(2)==_T("00")) 
		szChMoney+=_T("��");
	return szChMoney;
}
//���û�������
void QueryNode::set_fee_voice(int LineNo,Channel &channel,CString fee_str){
	//ת��Ϊ����д
	CString fee_dx = _T(rmb_dx(fee_str));
	//ѭ����ȡ�����ļ�
	for(int i = 0;i < fee_dx.GetLength();i=i+2)
	{
		LPTSTR voice_path = "";
		CString s(_T(fee_dx).Mid(i,2));
		if(myXml.findVoiceNameByContent(channel.CurrNodeId,s,voice_path))
			AddIndexPlayFile(LineNo,voice_path);

	}

}
void QueryNode::ResetLine(int LineNo,Channel &channel)
{
  if( channel.nType == CHTYPE_TRUNK )
  {
    //	FeedPower(channel.ConnectToLine);
    HangUp(LineNo);
    Sig_ResetCheck(LineNo);
    StartSigCheck(LineNo);
  }
  else if( channel.nType == CHTYPE_USER )
  {


  }

  InitDtmfBuf(LineNo);
  channel.UsableFlag = TRUE;
  channel.ConnectToLine = -1;
  channel.State = CH_FREE;
  channel.TelLength = 0;
  channel.DTMFCode[0] = NULL;
  channel.nTimeElapse = 0;
  playCount = 0;
  playCount2 = 0;
  state = CH_QUERY;
  memset(inputCode,0,100);
  bFlag1 = FALSE;
  bFlag2 = FALSE;

  //д��־
  CTime tm;
  CString strTime;
  tm=CTime::GetCurrentTime();
  strTime = tm.Format(_T("%Y-%m-%d %X"));
  myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
}
