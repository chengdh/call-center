#pragma once

#import "c:\program files\common files\system\ado\msado15.dll" \
	no_namespace rename("EOF","EndOfFile")

class MySql
{
public:
	MySql(void);
	~MySql(void);

public:
	static _ConnectionPtr pConn;
	static _ConnectionPtr pOracleConn;
	static int i, j;
	int findfrightIDCount,StateFromFreightCount,MoneyStateFromFreightCount,MoneyFromFreightCount;
public:
	_ConnectionPtr openConnection(_bstr_t strConn,_bstr_t uid,_bstr_t pwd,int cursorType);
	_ConnectionPtr openOracleConnection(_bstr_t strConn, _bstr_t uid, _bstr_t pwd, int cursorType);//创建0RACLE连接
	BOOL findUserId(CString UserId,_ConnectionPtr pConn);
	BOOL checkPassword(CString UserId,CString strPwd,_ConnectionPtr pConn);
	_RecordsetPtr getUserDetails(_ConnectionPtr pConn);
	_RecordsetPtr getUserDetailsByUserId(CString strUserId,_ConnectionPtr pConn);

	BOOL updateUserRecentLoginTime(CString strUserId,CString loginTime,_ConnectionPtr pConn);
	BOOL updateUserRecentLogoutTime(CString strUserId,CString logoutTime,_ConnectionPtr pConn);

	BOOL findFreightId(CString FreightId,_ConnectionPtr pConn,CString strSql);
	CString getStateFromFreight(CString FreightId,_ConnectionPtr pConn);
	CString getMoneyStateFromFreight(CString FreightId, _ConnectionPtr pConn);
	_RecordsetPtr getFreightDetailsByType(CString type,_ConnectionPtr pConn);
	_RecordsetPtr getFreightDetailsByState(CString state,_ConnectionPtr pConn);
	_RecordsetPtr getFreightDetailsByFreightId(CString freightId,_ConnectionPtr pConn);
	CString getMoneyFromFreight(CString FreightId,_ConnectionPtr pConn);

};
