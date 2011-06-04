#include "../include/OdbcInterface.h"

bool OdbcInterface::Init()
{
  // If Windows, then default to odbc32 and try odbc second
  // Otherwise try odbc first and try odbc32 second
#ifdef __WIN32__
  bool bLoaded = m_OdbcDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("odbc32")));
#else
  bool bLoaded = m_OdbcDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("odbc")));
#endif
  if (!bLoaded)
  {
#ifdef __WIN32__
    bLoaded = m_OdbcDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("odbc")));
#else
    bLoaded = m_OdbcDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("odbc32")));
#endif
    if (!bLoaded)
      return false;
  }

  wxString symbol = wxT("SQLAllocHandle");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLAllocHandle = (SQLAllocHandleType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLSetEnvAttr");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLSetEnvAttr = (SQLSetEnvAttrType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLFreeHandle");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLFreeHandle = (SQLFreeHandleType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLConnect");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLConnect = (SQLConnectType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLDriverConnect");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLDriverConnect = (SQLDriverConnectType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLDisconnect");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLDisconnect = (SQLDisconnectType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLSetConnectAttr");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLSetConnectAttr = (SQLSetConnectAttrType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLEndTran");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLEndTran = (SQLEndTranType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLPrepare");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLPrepare = (SQLPrepareType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLFreeStmt");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLFreeStmt = (SQLFreeStmtType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLTables");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLTables = (SQLTablesType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLFetch");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLFetch = (SQLFetchType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLGetData");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLGetData = (SQLGetDataType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLColumns");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLColumns = (SQLColumnsType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLGetDiagRec");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLGetDiagRec = (SQLGetDiagRecType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLNumParams");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLNumParams = (SQLNumParamsType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLExecute");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLExecute = (SQLExecuteType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLParamData");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLParamData = (SQLParamDataType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLPutData");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLPutData = (SQLPutDataType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLRowCount");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLRowCount = (SQLRowCountType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLNumResultCols");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLNumResultCols = (SQLNumResultColsType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLDescribeParam");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLDescribeParam = (SQLDescribeParamType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLBindParameter");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLBindParameter = (SQLBindParameterType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLColAttributes");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLColAttributes = (SQLColAttributesType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLColAttribute");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLColAttribute = (SQLColAttributeType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("SQLDescribeCol");
  if (m_OdbcDLL.HasSymbol(symbol))
  {
    m_pSQLDescribeCol = (SQLDescribeColType)m_OdbcDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  return true;
}

