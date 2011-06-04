#include "../include/MysqlInterface.h"

bool MysqlInterface::Init()
{
#ifdef __WIN32__
  bool bLoaded = m_MysqlDLL.Load(wxT("libmySQL.dll"), wxDL_VERBATIM);
#else
  bool bLoaded = m_MysqlDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("mysqlclient")));
#endif
  if (!bLoaded)
  {
    return false;
  }

  wxString symbol = wxT("mysql_server_end");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlServerEnd = (MysqlServerEndType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_init");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlInit = (MysqlInitType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_real_connect");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlRealConnect = (MysqlRealConnectType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_real_query");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlRealQuery = (MysqlRealQueryType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_error");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlError = (MysqlErrorType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_errno");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlErrno = (MysqlErrnoType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_close");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlClose = (MysqlCloseType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_autocommit");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlAutoCommit = (MysqlAutoCommitType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_commit");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlCommit = (MysqlCommitType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_rollback");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlRollback = (MysqlRollbackType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_query");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlQuery = (MysqlQueryType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_affected_rows");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlAffectedRows = (MysqlAffectedRowsType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_init");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtInit = (MysqlStmtInitType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_prepare");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtPrepare = (MysqlStmtPrepareType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_execute");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtExecute = (MysqlStmtExecuteType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_error");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtError = (MysqlStmtErrorType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_errno");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtErrno = (MysqlStmtErrnoType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_free_result");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtFreeResult = (MysqlStmtFreeResultType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_close");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtClose = (MysqlStmtCloseType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_list_tables");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlListTables = (MysqlListTablesType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_fetch_row");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlFetchRow = (MysqlFetchRowType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_free_result");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlFreeResult = (MysqlFreeResultType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_get_server_version");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlGetServerVersion = (MysqlGetServerVersionType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_result_metadata");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtResultMetadata = (MysqlStmtResultMetadataType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_num_fields");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlNumFields = (MysqlNumFieldsType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_param_count");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtParamCount = (MysqlStmtParamCountType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_bind_param");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtBindParam = (MysqlStmtBindParamType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_fetch");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtFetch = (MysqlStmtFetchType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  symbol = wxT("mysql_stmt_bind_result");
  if (m_MysqlDLL.HasSymbol(symbol))
  {
    m_pMysqlStmtBindResult = (MysqlStmtBindResultType)m_MysqlDLL.GetSymbol(symbol);
  }
  else
  {
    return false;
  }

  return true;
}
