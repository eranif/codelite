#include "../include/MysqlInterface.h"

namespace
{
template <typename SigPtr>
SigPtr loadSymbol(wxDynamicLibrary& dll, const wxString& symbol)
{
    if (!dll.HasSymbol(symbol)) {
        throw wxT("Cannot find symbol ") + symbol;
    }
    return reinterpret_cast<SigPtr>(dll.GetSymbol(symbol));
}
} // namespace

bool MysqlInterface::Init()
{
#ifdef __WIN32__
    bool bLoaded = m_MysqlDLL.Load(wxT("libmariadb.dll"), wxDL_VERBATIM);
#else
    bool bLoaded = m_MysqlDLL.Load(wxDynamicLibrary::CanonicalizeName(wxT("mysqlclient")));
#endif
    if (!bLoaded) {
        throw wxString(wxT("dll not found"));
    }

    m_pMysqlServerEnd = loadSymbol<MysqlServerEndType>(m_MysqlDLL, wxT("mysql_server_end"));
    m_pMysqlInit = loadSymbol<MysqlInitType>(m_MysqlDLL, wxT("mysql_init"));
    m_pMysqlRealConnect = loadSymbol<MysqlRealConnectType>(m_MysqlDLL, wxT("mysql_real_connect"));
    m_pMysqlRealQuery = loadSymbol<MysqlRealQueryType>(m_MysqlDLL, wxT("mysql_real_query"));
    m_pMysqlError = loadSymbol<MysqlErrorType>(m_MysqlDLL, wxT("mysql_error"));
    m_pMysqlErrno = loadSymbol<MysqlErrnoType>(m_MysqlDLL, wxT("mysql_errno"));
    m_pMysqlClose = loadSymbol<MysqlCloseType>(m_MysqlDLL, wxT("mysql_close"));
    m_pMysqlAutoCommit = loadSymbol<MysqlAutoCommitType>(m_MysqlDLL, wxT("mysql_autocommit"));
    m_pMysqlCommit = loadSymbol<MysqlCommitType>(m_MysqlDLL, wxT("mysql_commit"));
    m_pMysqlRollback = loadSymbol<MysqlRollbackType>(m_MysqlDLL, wxT("mysql_rollback"));
    m_pMysqlQuery = loadSymbol<MysqlQueryType>(m_MysqlDLL, wxT("mysql_query"));
    m_pMysqlAffectedRows = loadSymbol<MysqlAffectedRowsType>(m_MysqlDLL, wxT("mysql_affected_rows"));
    m_pMysqlStmtInit = loadSymbol<MysqlStmtInitType>(m_MysqlDLL, wxT("mysql_stmt_init"));
    m_pMysqlStmtPrepare = loadSymbol<MysqlStmtPrepareType>(m_MysqlDLL, wxT("mysql_stmt_prepare"));
    m_pMysqlStmtExecute = loadSymbol<MysqlStmtExecuteType>(m_MysqlDLL, wxT("mysql_stmt_execute"));
    m_pMysqlStmtError = loadSymbol<MysqlStmtErrorType>(m_MysqlDLL, wxT("mysql_stmt_error"));
    m_pMysqlStmtErrno = loadSymbol<MysqlStmtErrnoType>(m_MysqlDLL, wxT("mysql_stmt_errno"));
    m_pMysqlStmtFreeResult = loadSymbol<MysqlStmtFreeResultType>(m_MysqlDLL, wxT("mysql_stmt_free_result"));
    m_pMysqlStmtClose = loadSymbol<MysqlStmtCloseType>(m_MysqlDLL, wxT("mysql_stmt_close"));
    m_pMysqlListTables = loadSymbol<MysqlListTablesType>(m_MysqlDLL, wxT("mysql_list_tables"));
    m_pMysqlFetchRow = loadSymbol<MysqlFetchRowType>(m_MysqlDLL, wxT("mysql_fetch_row"));
    m_pMysqlFreeResult = loadSymbol<MysqlFreeResultType>(m_MysqlDLL, wxT("mysql_free_result"));
    m_pMysqlGetServerVersion = loadSymbol<MysqlGetServerVersionType>(m_MysqlDLL, wxT("mysql_get_server_version"));
    m_pMysqlStmtResultMetadata = loadSymbol<MysqlStmtResultMetadataType>(m_MysqlDLL, wxT("mysql_stmt_result_metadata"));
    m_pMysqlNumFields = loadSymbol<MysqlNumFieldsType>(m_MysqlDLL, wxT("mysql_num_fields"));
    m_pMysqlStmtParamCount = loadSymbol<MysqlStmtParamCountType>(m_MysqlDLL, wxT("mysql_stmt_param_count"));
    m_pMysqlStmtBindParam = loadSymbol<MysqlStmtBindParamType>(m_MysqlDLL, wxT("mysql_stmt_bind_param"));
    m_pMysqlStmtFetch = loadSymbol<MysqlStmtFetchType>(m_MysqlDLL, wxT("mysql_stmt_fetch"));
    m_pMysqlStmtBindResult = loadSymbol<MysqlStmtBindResultType>(m_MysqlDLL, wxT("mysql_stmt_bind_result"));

    return true;
}
