#include "StdAfx.h"
#include ".\mysql.h"

MySql::MySql(void)
{
	findfrightIDCount=0;
	StateFromFreightCount=0;
	MoneyStateFromFreightCount=0;
	MoneyFromFreightCount=0;
}

MySql::~MySql(void)
{
}

int MySql::i = 0;
int MySql::j = 0;
_ConnectionPtr MySql::pConn = NULL;
_ConnectionPtr MySql::pOracleConn = NULL;
_ConnectionPtr MySql::openConnection(_bstr_t strConn, _bstr_t uid, _bstr_t pwd, int cursorType)
{	
	
	if( i > 1 )
	{		 
		return pConn;
	}

	pConn.CreateInstance(__uuidof(Connection));
	pConn->ConnectionTimeout = 5;
	try
	{
		pConn->Open(strConn,uid,pwd,cursorType);
	}
	catch(_com_error e)
	{
		AfxMessageBox("2"+e.Description());
	}
	i++;
	return pConn;
}



_ConnectionPtr MySql::openOracleConnection(_bstr_t strConn,_bstr_t uid,_bstr_t pwd,int cursorType)
{	
	//if(j > 0)
	//{		 
		//return pOracleConn;
	//}

	pOracleConn.CreateInstance(__uuidof(Connection));
	pOracleConn->ConnectionTimeout = 10;
	try
	{
		pOracleConn->Open(strConn, uid, pwd, cursorType);
	}
	catch(_com_error e)
	{
		AfxMessageBox("3"+e.Description());
	}
	j++;
	return pOracleConn;
}

BOOL MySql::findUserId(CString UserId, _ConnectionPtr pConn)
{
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	strSqlFind.Format(_T("select count(*) from UserDetails where UserId= '%s' "),UserId);
	pConn->AddRef();
	try
	{
		_RecordsetPtr pSet=pConn->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);
		int nCount;		
		if(!pSet->EndOfFile)
		{
			nCount=atol((LPCSTR)(_bstr_t)pSet->GetCollect(long(0)));
			if(nCount>0)
				return TRUE;
		}				
	}
	catch(_com_error e)
	{
		AfxMessageBox("4"+e.Description());
	}

	return FALSE;
}

BOOL MySql::checkPassword(CString UserId,CString strPwd,_ConnectionPtr pConn)
{
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	strSqlFind.Format(_T("select Pwd from UserDetails where UserId= '%s' "),UserId);
	pConn->AddRef();
	try
	{
		_RecordsetPtr pSet=pConn->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);	
		CString pwd;
		if(!pSet->EndOfFile)
		{
			pwd = (LPCSTR)(_bstr_t)pSet->GetCollect(long(0));
			if( strPwd == pwd )
				return TRUE;
		}				
	}
	catch(_com_error e)
	{
		AfxMessageBox("5"+e.Description());
	}

	return FALSE;
}

_RecordsetPtr MySql::getUserDetails(_ConnectionPtr pConn)
{
	_RecordsetPtr pSet;
	pSet.CreateInstance(__uuidof(Recordset));
	CString strSql="Select * from UserDetails";
	BSTR bstrSql=strSql.AllocSysString();
	_variant_t vDispatch((IDispatch*)pConn);
	pConn->AddRef();
	try
	{
		pSet->Open(bstrSql,vDispatch,adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(const _com_error e)
	{
		 AfxMessageBox("6"+e.Description());
	}	
	return pSet;	
}

_RecordsetPtr MySql::getUserDetailsByUserId(CString strUserId,_ConnectionPtr pConn)
{
	_RecordsetPtr pSet;
	pSet.CreateInstance(__uuidof(Recordset));
	CString strSql="Select * from UserDetails where UserId='"+strUserId+"';";
	BSTR bstrSql=strSql.AllocSysString();
	_variant_t vDispatch((IDispatch*)pConn);
	pConn->AddRef();
	try
	{
		pSet->Open(bstrSql,vDispatch,adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(const _com_error e)
	{
		 AfxMessageBox("7"+e.Description());
	}	
	return pSet;	
}

BOOL MySql::findFreightId(CString FreightId, _ConnectionPtr pConn, CString strSql)
{
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	_ConnectionPtr tmpFindFreightID;
	tmpFindFreightID.CreateInstance(__uuidof(Connection));
	tmpFindFreightID->ConnectionString = _T("DSN=lmis");
	try
	{
		tmpFindFreightID->Open("","","",adConnectUnspecified);
	}
	catch(_com_error e)
	{
		AfxMessageBox("8"+e.Description());
		return false;
	}
	
	//CString m_strConn1 =_T("Provider=MSDASQL.1;Password=lmis;User ID=lmis;STMT=SET NAMES 'GBK';Extended Properties=\"DRIVER={MySQL ODBC 3.51 Driver};DB=lmis;SERVER=218.28.134.125;UID=lmis;PASSWORD=lmis;PORT=3306;\"");  
	//CString m_strConn1 ="Provider=MSDAORA.1;Data Source=zhao;User Id=lmis;Password=lmis";
	//if(tmpFindFreightID == NULL)
	//{
	//	tmpFindFreightID = openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);
		//findfrightIDCount = 1;
	//}else
	//{
		//findfrightIDCount++;
	//}
		//strSqlFind.Format(_T("select count(*) from FREIGHTLIST where FREIGHTNO='%s' "),FreightId);
	strSqlFind = strSql + CString("'") +FreightId + CString("'");
	tmpFindFreightID->AddRef();
	try
	{
		_RecordsetPtr pSet=tmpFindFreightID->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);
		int nCount;		
		if(!pSet->EndOfFile)
		{
			nCount=atol((LPCSTR)(_bstr_t)pSet->GetCollect(long(0)));
			if(nCount > 0)
			{
				//findfrightIDCount--;
				//if(findfrightIDCount == 0)
				//{
					tmpFindFreightID->Close();
					tmpFindFreightID = NULL;
				//}
				return TRUE;
			}
		}				
	}
	catch(_com_error e)
	{
		AfxMessageBox("8"+e.Description());
	}
	return FALSE;
}

CString MySql::getStateFromFreight(CString FreightId, _ConnectionPtr pConn)
{
	CString strState = "";
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	_ConnectionPtr StateFromFreight;
	StateFromFreight.CreateInstance(__uuidof(Connection));
	StateFromFreight->ConnectionString = _T("DSN=lmis");
	try
	{
		StateFromFreight->Open("","","",adConnectUnspecified);
	}catch(_com_error e)
	{
		AfxMessageBox("9"+ e.Description());
		return strState;
	}
	//CString m_strConn1 =_T("Provider=MSDASQL.1;Password=lmis;User ID=lmis;STMT=SET NAMES 'GBK';Extended Properties=\"DRIVER={MySQL ODBC 3.51 Driver};DB=lmis;SERVER=218.28.134.125;UID=lmis;PASSWORD=lmis;PORT=3306;\"");  
	//CString m_strConn1 ="Provider=MSDAORA.1;Data Source=zhao;User Id=lmis;Password=lmis";
	//if(StateFromFreight == NULL)
	//{
			//StateFromFreight = openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);
			//StateFromFreightCount=1;
	//}else
	//{
		//StateFromFreightCount++;
	//}
	strSqlFind.Format(_T("select state from view_bills where bill_no='%s' "),FreightId);
	StateFromFreight->AddRef();
	try
	{
		_RecordsetPtr pSet=StateFromFreight->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);			
		if(!pSet->EndOfFile)
		{
		//	pSet->MoveFirst();
			strState = (LPCTSTR)(_bstr_t)pSet->GetCollect(long(0));			
		}	
		
	}	
	catch(_com_error e)
	{
		AfxMessageBox("9"+e.Description());
	}
	//StateFromFreightCount--;
	//if(StateFromFreightCount == 0)
	//{
		StateFromFreight->Close();
		StateFromFreight = NULL;
	//}	
	return translate_bill_state(strState);
}

//转换运单状态
CString MySql::translate_bill_state(CString origin_state)
{
	CString ret_state = "";
	//已开票、已装车、已发货 ----> 已发货
	if(origin_state == "billed" || origin_state == "loaded" || origin_state == "sended")
		ret_state = "sended";
	//已到货、已分货 -----------》未提货
	else if(origin_state == "reached" || origin_state == "distributed")
		ret_state = "not_deliveried";
	//已提货 已日结 已返款 已到账 ------> 已提货
	else if(origin_state == "deliveried" || origin_state == "settlemented" || origin_state == "refunded" || origin_state == "refunded_confirmed")
		ret_state = "deliveried";
	//等待支付货款 ----> 已提货待提款
	else if(origin_state == "payment_listed")
		ret_state = "payment_listed";
	//已支付 已过账 ----》 已转账
	else if(origin_state == "paid" || origin_state == "posted")
		ret_state = "paid";
	//已退货 ------》已退货
	else if(origin_state == "returned")
		ret_state ="returned";

	//已注销 已作废 -----》已作废
	else if(origin_state == "invalidated" || origin_state == "canceled")
		ret_state = "canceled";

	return ret_state;

}

CString MySql::getMoneyStateFromFreight(CString FreightId, _ConnectionPtr pConn)
{
	CString strMoneyState = "";
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	_ConnectionPtr MoneyStateFromFreight;
	MoneyStateFromFreight.CreateInstance(__uuidof(Connection));
	MoneyStateFromFreight->ConnectionString = _T("DSN=lmis");
	try
	{
		MoneyStateFromFreight->Open("","","",adConnectUnspecified);
	}
	catch(_com_error e)
	{
		AfxMessageBox("10"+e.Description());
		return strMoneyState;
	}
	//CString m_strConn1 =_T("Provider=MSDASQL.1;Password=lmis;User ID=lmis;STMT=SET NAMES 'GBK';Extended Properties=\"DRIVER={MySQL ODBC 3.51 Driver};DB=lmis;SERVER=218.28.134.125;UID=lmis;PASSWORD=lmis;PORT=3306;\"");  
	//CString m_strConn1 ="Provider=MSDAORA.1;Data Source=zhao;User Id=lmis;Password=lmis";
	//if(MoneyStateFromFreight==NULL)
	//{
		//MoneyStateFromFreight = openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);
		//MoneyStateFromFreightCount=1;
	//}else
	//{
		//MoneyStateFromFreightCount++;
	//}
	strSqlFind.Format(_T("select state from view_bills where bill_no='%s' "),FreightId);
	MoneyStateFromFreight->AddRef();
	try
	{
		_RecordsetPtr pSet=MoneyStateFromFreight->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);			
		if(!pSet->EndOfFile)
		{
		//	pSet->MoveFirst();
			strMoneyState = (LPCTSTR)(_bstr_t)pSet->GetCollect(long(0));			
		}	
		
	}
	catch(_com_error e)
	{
		AfxMessageBox("10"+e.Description());
	}
	//MoneyStateFromFreightCount--;
	//if(MoneyStateFromFreightCount == 0)
	//{
		MoneyStateFromFreight->Close();
		MoneyStateFromFreight = NULL;
	//}	
	return strMoneyState;
}
//获取运费信息
CString MySql::getMoneyFromFreight(CString FreightId, _ConnectionPtr pConn)
{
	CString strMoney = "";
	CString strSqlFind = "";
	_variant_t RecordsAffected = 0;
	_ConnectionPtr MoneyFromFreight;
	double money = 0.0;
	MoneyFromFreight.CreateInstance(__uuidof(Connection));
	MoneyFromFreight->ConnectionString = "DSN=lmis";
	try
	{
		MoneyFromFreight->Open("","","",adConnectUnspecified);
	}
	catch(_com_error e)
	{
		AfxMessageBox(""+e.Description());
		return strMoney;
	}
	

	//CString m_strConn1 =_T("Provider=MSDASQL.1;Password=lmis;User ID=lmis;STMT=SET NAMES 'GBK';Extended Properties=\"DRIVER={MySQL ODBC 3.51 Driver};DB=lmis;SERVER=218.28.134.125;UID=lmis;PASSWORD=lmis;PORT=3306;\"");  
	//CString m_strConn1 ="Provider=MSDAORA.1;Data Source=zhao;User Id=lmis;Password=lmis";
	//if(MoneyFromFreight == NULL)
	//{
	//		MoneyFromFreight = openOracleConnection(_com_util::ConvertStringToBSTR(m_strConn1),"","",adOpenUnspecified);
			//MoneyFromFreightCount=1;
	//}
	//else
	//{
		//MoneyFromFreightCount++;
	//}
	strSqlFind.Format(_T("select goods_fee from view_bills where bill_no='%s'"),FreightId);
	MoneyFromFreight->AddRef();
	try
	{
		_RecordsetPtr pSet=MoneyFromFreight->Execute(strSqlFind.AllocSysString(),&RecordsAffected,adCmdText);			
		if(!pSet->EndOfFile)
		{
		//	pSet->MoveFirst();
			
			//strMoney = (LPCTSTR)(_bstr_t)pSet->GetCollect(long(0));
			money = pSet->GetCollect(long(0));
			strMoney.Format(_T("%18.0f"), money*100);
			//strMoney = (LPCTSTR)(_bstr_t)pSet->GetCollect(long(0));	
		}	
		//pConn->Close();
	}
	catch(_com_error e)
	{
		AfxMessageBox("11"+e.Description());
	}
	//MoneyFromFreightCount--;
	//if(MoneyFromFreightCount == 0)
	//{
		MoneyFromFreight->Close();
		MoneyFromFreight = NULL;
	//}	
	return strMoney;
}

_RecordsetPtr MySql::getFreightDetailsByType(CString type,_ConnectionPtr pConn)
{
	_RecordsetPtr pSet;
	CString strSql = "";
	pSet.CreateInstance(__uuidof(Recordset));
	if(type == _T("a"))
		strSql="Select * from CallCenter";
	else
		strSql="Select * from CallCenter where GOODSFEET="+type;

	BSTR bstrSql=strSql.AllocSysString();
	_variant_t vDispatch((IDispatch*)pConn);
	pConn->AddRef();
	try
	{
		pSet->Open(bstrSql,vDispatch,adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(const _com_error e)
	{
		 AfxMessageBox("12"+e.Description());
	}	
	return pSet;	
}

_RecordsetPtr MySql::getFreightDetailsByState(CString state,_ConnectionPtr pConn)
{
	_RecordsetPtr pSet;
	CString strSql = "";
	pSet.CreateInstance(__uuidof(Recordset));
	if(state == _T("a"))
		strSql="Select * from FREIGHTLIST";
	else
		strSql="Select * from FREIGHTLIST where IFDRAWED='"+state+"'";

	BSTR bstrSql=strSql.AllocSysString();
	_variant_t vDispatch((IDispatch*)pConn);
	pConn->AddRef();
	try
	{
		pSet->Open(bstrSql,vDispatch,adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(const _com_error e)
	{
		 AfxMessageBox("13"+e.Description());
	}	
	return pSet;	
}

_RecordsetPtr MySql::getFreightDetailsByFreightId(CString freightId,_ConnectionPtr pConn)
{
	_RecordsetPtr pSet;
	CString strSql = "";
	pSet.CreateInstance(__uuidof(Recordset));
	strSql="Select * from view_bills where bill_no='"+freightId+"'";	
	BSTR bstrSql=strSql.AllocSysString();
	_variant_t vDispatch((IDispatch*)pConn);
	pConn->AddRef();
	try
	{
		pSet->Open(bstrSql,vDispatch,adOpenDynamic,adLockOptimistic,adCmdText);
	}
	catch(const _com_error e)
	{
		 AfxMessageBox("14"+e.Description());
	}	
	return pSet;	
}

BOOL MySql::updateUserRecentLoginTime(CString strUserId, CString loginTime, _ConnectionPtr pConn)
{
	_variant_t RecordsAffected = 0;
	CString strSqlUpdate="";	
	strSqlUpdate.Format(_T("Update UserDetails set RecentLogin='%s' where UserId='%s';"),loginTime,strUserId.Trim());	//不可缺少from
	try
	{
		pConn->Execute(strSqlUpdate.AllocSysString(),&RecordsAffected,adCmdText);	
		return TRUE;
	}
	catch(_com_error e)
	{
		AfxMessageBox("15"+e.Description());
	}
	
	return FALSE;
}

BOOL MySql::updateUserRecentLogoutTime(CString strUserId, CString logoutTime, _ConnectionPtr pConn)
{
	_variant_t RecordsAffected = 0;
	CString strSqlUpdate="";	
	strSqlUpdate.Format(_T("Update UserDetails set RecentLogout='%s' where UserId='%s';"),logoutTime,strUserId.Trim());	//不可缺少from
	try
	{
		pConn->Execute(strSqlUpdate.AllocSysString(),&RecordsAffected,adCmdText);	
		return TRUE;
	}
	catch(_com_error e)
	{
		AfxMessageBox("16"+e.Description());
	}
	
	return FALSE;
}
