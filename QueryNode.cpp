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
    waitTime = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("WaitTime"));
    replayNum = myXml.getWaitTimeOrReplayNumByNodeId(channel.CurrNodeId,_T("ReplayNum"));
    bFlag1 = myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("MainVoice"),VoicePath1);
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
    case CH_QUERY:
      channel.nTimeElapse = 0;
      strState = _T("播放语音");
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
    case CH_WAITDIAL1:
      strState = _T("等待拨号");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if( CheckIndexPlayFile(LineNo) )
      {
        StopIndexPlayFile(LineNo);
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
    case CH_GETDIAL1:
      strState = _T("接受用户拨号");
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
    case CH_GETDIAL2:
      strState = _T("接受运单编号");
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
          //此处判断运单是否存在
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
    case CH_PLAYRESULT:
      channel.nTimeElapse = 0;
      channel.TelLength = 0;
      strState = _T("播放语音");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      RsetIndexPlayFile(LineNo);

      strMoney = _T("");
      strState = _T("");
      strState = mySql.getStateFromFreight(inputCode,pConn1);
      strMoneyState = mySql.getMoneyStateFromFreight(inputCode, pConn1);
      if( playCount2 == 0 )
      {
        if(strState.Trim() == CString("billed") || strState.Trim() == CString("loaded") || strState.Trim() == CString("shipped"))
        {
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint9"),VoicePath7) ) //数据查询中,请等待
          {
            AddIndexPlayFile(LineNo,VoicePath7);
          }
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint7"),VoicePath7) ) //货物尚未到达
          {
            AddIndexPlayFile(LineNo,VoicePath7);
          }
        }

        if(strState.Trim() == CString("reached"))
        {
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint9"),VoicePath8) )
          {
            AddIndexPlayFile(LineNo,VoicePath8);
          }
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint8"),VoicePath8) ) //货物已到达,尚未提货
          {
            AddIndexPlayFile(LineNo,VoicePath8);
          }
        }

        if(strState.Trim() == CString("returned"))  //货物已退回
        {
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint9"),VoicePath8) )
          {
            AddIndexPlayFile(LineNo,VoicePath8);
          }
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint10"),VoicePath10) )
          {
            AddIndexPlayFile(LineNo,VoicePath10);
          }
        }
        //报货款
        if(strState.Trim() == CString("deliveried") || strState.Trim() == CString("settlemented") || strState.Trim() == CString("refunded") ||   strState.Trim() == CString("refunded_confirmed") || strState.Trim() == CString("payment_listed") || strState.Trim() == CString("paid") || strState.Trim() == CString("posted"))
        {
          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint9"),VoicePath8) )
          {
            AddIndexPlayFile(LineNo,VoicePath8);
          }
        }
        //货款尚未到账
        if(strState.Trim() == CString("deliveried") || strState.Trim() == CString("settlemented") || strState.Trim() == CString("refunded")  )  //已提货
        {

          if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint2"),VoicePath3) )
          {
            AddIndexPlayFile(LineNo,VoicePath3);
          }

        }
        //已到账
        if(strState.Trim() == CString("refunded_confirmed") ||strState.Trim() == CString("payment_listed"))
        {

          if(channel.CurrNodeId == 21)  //现金支付
          {
            if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint3"),VoicePath3) )  //已经到账
            {
              AddIndexPlayFile(LineNo,VoicePath3);
            }
          }
          if(channel.CurrNodeId == 22) //银行转账
          {
            if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint2"),VoicePath3) ) //尚未到账
            {
              AddIndexPlayFile(LineNo,VoicePath3);
            }
          }

        }
        //已提款
        if(strState.Trim() == CString("paid") || strState.Trim() == CString("posted"))
        {
          if(channel.CurrNodeId == 21)
          {
            if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint4"),VoicePath3) ) //已经提走
            {
              AddIndexPlayFile(LineNo,VoicePath3);
            }
          }
          if(channel.CurrNodeId == 22)
          {
            if(	myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("Hint11"),VoicePath3) ) //已转入账户
            {
              AddIndexPlayFile(LineNo,VoicePath3);
            }
          }


        }
      }

      if( myXml.findVoiceNameByNodeId(channel.CurrNodeId,_T("HintVoice2"),HintVoice2) )
      {
        AddIndexPlayFile(LineNo,HintVoice2);
      }
      StartIndexPlayFile(LineNo);
      playCount2++;
      state = CH_WAITDIAL3;
      break;
    case CH_WAITDIAL3:
      strState = _T("等待拨号");
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
      strState = _T("接受用户拨号");
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
      strState = _T("错误输入");
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
      strState = _T("错误输入");
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
      strState = _T("等待挂机");
      tmpState = pList1->GetItemText(LineNo,2);
      if( tmpState != strState )
        pList1->SetItemText(LineNo,2,strState);

      if(CheckPlayEnd(LineNo))
      {
        StopPlayFile(LineNo);
        ResetLine(LineNo,channel);

        strState = _T("空闲");
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

  //写日志
  CTime tm;
  CString strTime;
  tm=CTime::GetCurrentTime();
  strTime = tm.Format(_T("%Y-%m-%d %X"));
  myXml.insertTextToLogItem(channel.strItemId,strTime,_T("EndTime"),channel.ctStartTime);
}
