#include "../include/MysqlPreparedStatementWrapper.h"
#include "../include/MysqlPreparedStatementResultSet.h"
#include "../include/MysqlDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"

#include "errmsg.h"

MysqlPreparedStatementWrapper::MysqlPreparedStatementWrapper(MysqlInterface* pInterface, MYSQL_STMT* pStatement)
 : DatabaseErrorReporter()
{
  m_pInterface = pInterface;
  m_pStatement = pStatement;
}

MysqlPreparedStatementWrapper::~MysqlPreparedStatementWrapper()
{
  Close();
}

void MysqlPreparedStatementWrapper::Close()
{
  if (m_pStatement != NULL)
  {
    m_pInterface->GetMysqlStmtClose()(m_pStatement);
    m_pStatement = NULL;
  }
}

// set field
void MysqlPreparedStatementWrapper::SetParam(int nPosition, int nValue)
{
  m_Parameters.SetParam(nPosition, nValue);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition, double dblValue)
{
  m_Parameters.SetParam(nPosition, dblValue);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition, const wxString& strValue)
{
  m_Parameters.SetParam(nPosition, strValue);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition)
{
  m_Parameters.SetParam(nPosition);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition, const void* pData, long nDataLength)
{
  m_Parameters.SetParam(nPosition, pData, nDataLength);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition, const wxDateTime& dateValue)
{
  m_Parameters.SetParam(nPosition, dateValue);
}

void MysqlPreparedStatementWrapper::SetParam(int nPosition, bool bValue)
{
  m_Parameters.SetParam(nPosition, bValue);
}

int MysqlPreparedStatementWrapper::GetParameterCount()
{
  return m_pInterface->GetMysqlStmtParamCount()(m_pStatement);
}

int MysqlPreparedStatementWrapper::RunQuery()
{
  MYSQL_BIND* pBoundParameters = m_Parameters.GetMysqlParameterBindings();

  int nBindReturn = m_pInterface->GetMysqlStmtBindParam()(m_pStatement, pBoundParameters);
  if (nBindReturn != 0)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
    wxDELETEA(pBoundParameters);
    ThrowDatabaseException();
    return DATABASE_LAYER_QUERY_RESULT_ERROR;
  }
  else
  {
    int nReturn = m_pInterface->GetMysqlStmtExecute()(m_pStatement);
    if (nReturn != 0)
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
      SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
      wxDELETEA(pBoundParameters);
      ThrowDatabaseException();
      return DATABASE_LAYER_QUERY_RESULT_ERROR;
    }
  }
  wxDELETEA(pBoundParameters);

  return (m_pStatement->affected_rows);
}

DatabaseResultSet* MysqlPreparedStatementWrapper::RunQueryWithResults()
{
  MysqlPreparedStatementResultSet* pResultSet = NULL;
  MYSQL_BIND* pBoundParameters = m_Parameters.GetMysqlParameterBindings();

  if (m_pInterface->GetMysqlStmtBindParam()(m_pStatement, pBoundParameters))
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
    wxDELETEA(pBoundParameters);
    ThrowDatabaseException();
    return NULL;
  }
  else
  {
    if (m_pInterface->GetMysqlStmtExecute()(m_pStatement) != 0)
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
      SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
      wxDELETEA(pBoundParameters);
      ThrowDatabaseException();
      return NULL;
    }
    else
    {
      pResultSet = new MysqlPreparedStatementResultSet(m_pInterface, m_pStatement);
      if (pResultSet)
        pResultSet->SetEncoding(GetEncoding());
    }
  }
  wxDELETEA(pBoundParameters);;
  
  return pResultSet;
}

