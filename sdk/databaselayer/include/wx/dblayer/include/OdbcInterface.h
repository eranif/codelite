#ifndef __ODBC_INTERFACES_H__
#define __ODBC_INTERFACES_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynlib.h"

#include <sql.h>
#include <sqlext.h>

typedef SQLRETURN (SQL_API *SQLAllocHandleType)(SQLSMALLINT, SQLHANDLE, SQLHANDLE*);
typedef SQLRETURN (SQL_API *SQLSetEnvAttrType)(SQLHENV, SQLINTEGER, SQLPOINTER, SQLINTEGER);
typedef SQLRETURN (SQL_API *SQLFreeHandleType)(SQLSMALLINT, SQLHANDLE);
typedef SQLRETURN (SQL_API *SQLConnectType)(SQLHDBC, SQLTCHAR*, SQLSMALLINT,
  SQLTCHAR*, SQLSMALLINT, SQLTCHAR*, SQLSMALLINT);
typedef SQLRETURN (SQL_API *SQLDriverConnectType)(SQLHDBC, SQLHWND, SQLTCHAR*, 
  SQLSMALLINT, SQLTCHAR*, SQLSMALLINT, SQLSMALLINT*, SQLUSMALLINT);
typedef SQLRETURN (SQL_API *SQLDisconnectType)(SQLHDBC);
typedef SQLRETURN (SQL_API *SQLSetConnectAttrType)(SQLHDBC, SQLINTEGER, SQLPOINTER, SQLINTEGER);
typedef SQLRETURN (SQL_API *SQLEndTranType)(SQLSMALLINT, SQLHANDLE, SQLSMALLINT);
typedef SQLRETURN (SQL_API *SQLPrepareType)(SQLHSTMT, SQLTCHAR*, SQLINTEGER);
typedef SQLRETURN (SQL_API *SQLFreeStmtType)(SQLHSTMT, SQLUSMALLINT);
typedef SQLRETURN (SQL_API *SQLTablesType)(SQLHSTMT, SQLTCHAR*, SQLSMALLINT, SQLTCHAR*,
  SQLSMALLINT, SQLTCHAR*, SQLSMALLINT, SQLTCHAR*, SQLSMALLINT);
typedef SQLRETURN (SQL_API *SQLFetchType)(SQLHSTMT);
typedef SQLRETURN (SQL_API *SQLGetDataType)(SQLHSTMT, SQLUSMALLINT, SQLSMALLINT,
  SQLPOINTER, SQLLEN, SQLLEN*);
typedef SQLRETURN (SQL_API *SQLColumnsType)(SQLHSTMT, SQLTCHAR*, SQLSMALLINT, SQLTCHAR*,
  SQLSMALLINT, SQLTCHAR*, SQLSMALLINT, SQLTCHAR*, SQLSMALLINT);
typedef SQLRETURN (SQL_API *SQLGetDiagRecType)(SQLSMALLINT, SQLHANDLE, SQLSMALLINT, SQLTCHAR*,
  SQLINTEGER*, SQLTCHAR*, SQLSMALLINT, SQLSMALLINT*);
typedef SQLRETURN (SQL_API *SQLNumParamsType)(SQLHSTMT, SQLSMALLINT*);
typedef SQLRETURN (SQL_API *SQLExecuteType)(SQLHSTMT);
typedef SQLRETURN (SQL_API *SQLParamDataType)(SQLHSTMT, SQLPOINTER*);
typedef SQLRETURN (SQL_API *SQLPutDataType)(SQLHSTMT, SQLPOINTER, SQLLEN);
typedef SQLRETURN (SQL_API *SQLRowCountType)(SQLHSTMT, SQLLEN*);
typedef SQLRETURN (SQL_API *SQLNumResultColsType)(SQLHSTMT, SQLSMALLINT*);
typedef SQLRETURN (SQL_API *SQLDescribeParamType)(SQLHSTMT, SQLUSMALLINT, SQLSMALLINT*,
  SQLULEN*, SQLSMALLINT*, SQLSMALLINT*);
typedef SQLRETURN (SQL_API *SQLBindParameterType)(SQLHSTMT, SQLUSMALLINT, SQLSMALLINT,
  SQLSMALLINT, SQLSMALLINT, SQLULEN, SQLSMALLINT, SQLPOINTER, SQLLEN, SQLLEN*);
typedef SQLRETURN (SQL_API *SQLColAttributesType)(SQLHSTMT, SQLUSMALLINT, SQLUSMALLINT,
  SQLPOINTER, SQLSMALLINT, SQLSMALLINT*, SQLLEN*);
typedef SQLRETURN (SQL_API *SQLColAttributeType)(SQLHSTMT, SQLUSMALLINT, SQLUSMALLINT,
  SQLPOINTER, SQLSMALLINT, SQLSMALLINT*, SQLPOINTER);
typedef SQLRETURN (SQL_API *SQLDescribeColType)(SQLHSTMT, SQLUSMALLINT, SQLTCHAR*, SQLSMALLINT,
  SQLSMALLINT*, SQLSMALLINT*, SQLULEN*, SQLSMALLINT*, SQLSMALLINT*);


class OdbcInterface {
public:
  OdbcInterface() { }
  bool Init();

  SQLAllocHandleType GetSQLAllocHandle() { return m_pSQLAllocHandle; }
  SQLSetEnvAttrType GetSQLSetEnvAttr() { return m_pSQLSetEnvAttr; }
  SQLFreeHandleType GetSQLFreeHandle() { return m_pSQLFreeHandle; }
  SQLConnectType GetSQLConnect() { return m_pSQLConnect; }
  SQLDriverConnectType GetSQLDriverConnect() { return m_pSQLDriverConnect; }
  SQLDisconnectType GetSQLDisconnect() { return m_pSQLDisconnect; }
  SQLSetConnectAttrType GetSQLSetConnectAttr() { return m_pSQLSetConnectAttr; }
  SQLEndTranType GetSQLEndTran() { return m_pSQLEndTran; }
  SQLPrepareType GetSQLPrepare() { return m_pSQLPrepare; }
  SQLFreeStmtType GetSQLFreeStmt() { return m_pSQLFreeStmt; }
  SQLTablesType GetSQLTables() { return m_pSQLTables; }
  SQLFetchType GetSQLFetch() { return m_pSQLFetch; }
  SQLGetDataType GetSQLGetData() { return m_pSQLGetData; }
  SQLColumnsType GetSQLColumns() { return m_pSQLColumns; }
  SQLGetDiagRecType GetSQLGetDiagRec() { return m_pSQLGetDiagRec; }
  SQLNumParamsType GetSQLNumParams() { return m_pSQLNumParams; }
  SQLExecuteType GetSQLExecute() { return m_pSQLExecute; }
  SQLParamDataType GetSQLParamData() { return m_pSQLParamData; }
  SQLPutDataType GetSQLPutData() { return m_pSQLPutData; }
  SQLRowCountType GetSQLRowCount() { return m_pSQLRowCount; }
  SQLNumResultColsType GetSQLNumResultCols() { return m_pSQLNumResultCols; }
  SQLDescribeParamType GetSQLDescribeParam() { return m_pSQLDescribeParam; }
  SQLBindParameterType GetSQLBindParameter() { return m_pSQLBindParameter; }
  SQLColAttributesType GetSQLColAttributes() { return m_pSQLColAttributes; }
  SQLColAttributeType GetSQLColAttribute() { return m_pSQLColAttribute; }
  SQLDescribeColType GetSQLDescribeCol() { return m_pSQLDescribeCol; }


private:
  wxDynamicLibrary m_OdbcDLL;

  SQLAllocHandleType m_pSQLAllocHandle;
  SQLSetEnvAttrType m_pSQLSetEnvAttr;
  SQLFreeHandleType m_pSQLFreeHandle;
  SQLConnectType m_pSQLConnect;
  SQLDriverConnectType m_pSQLDriverConnect;
  SQLDisconnectType m_pSQLDisconnect;
  SQLSetConnectAttrType m_pSQLSetConnectAttr;
  SQLEndTranType m_pSQLEndTran;
  SQLPrepareType m_pSQLPrepare;
  SQLFreeStmtType m_pSQLFreeStmt;
  SQLTablesType m_pSQLTables;
  SQLFetchType m_pSQLFetch;
  SQLGetDataType m_pSQLGetData;
  SQLColumnsType m_pSQLColumns;
  SQLGetDiagRecType m_pSQLGetDiagRec;
  SQLNumParamsType m_pSQLNumParams;
  SQLExecuteType m_pSQLExecute;
  SQLParamDataType m_pSQLParamData;
  SQLPutDataType m_pSQLPutData;
  SQLRowCountType m_pSQLRowCount;
  SQLNumResultColsType m_pSQLNumResultCols;
  SQLDescribeParamType m_pSQLDescribeParam;
  SQLBindParameterType m_pSQLBindParameter;
  SQLColAttributesType m_pSQLColAttributes;
  SQLColAttributeType m_pSQLColAttribute;
  SQLDescribeColType m_pSQLDescribeCol;
};

#endif // __ODBC_INTERFACES_H__
