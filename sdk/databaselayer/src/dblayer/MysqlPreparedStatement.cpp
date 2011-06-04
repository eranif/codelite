#include "../include/MysqlPreparedStatement.h"
#include "../include/MysqlDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"

MysqlPreparedStatement::MysqlPreparedStatement(MysqlInterface* pInterface)
 : PreparedStatement()
{
  m_pInterface = pInterface;
  m_Statements.clear();
}

MysqlPreparedStatement::MysqlPreparedStatement(MysqlInterface* pInterface, MYSQL_STMT* pStatement)
 : PreparedStatement()
{
  m_pInterface = pInterface;
  AddPreparedStatement(pStatement);
}

MysqlPreparedStatement::~MysqlPreparedStatement()
{
  Close();
}


void MysqlPreparedStatement::Close()
{
  CloseResultSets();

  // Free the statements
  MysqlStatementWrapperArray::iterator start = m_Statements.begin();
  MysqlStatementWrapperArray::iterator stop = m_Statements.end();

  while (start != stop)
  {
    if ((*start) != NULL)
    {
      MysqlPreparedStatementWrapper* pWrapper = (MysqlPreparedStatementWrapper*)(*start);
      wxDELETE(pWrapper);
      (*start) = NULL;
    }
    start++;
  }
}

void MysqlPreparedStatement::AddPreparedStatement(MYSQL_STMT* pStatement)
{
  MysqlPreparedStatementWrapper* pStatementWrapper = new MysqlPreparedStatementWrapper(m_pInterface, pStatement);
  if (pStatementWrapper)
    pStatementWrapper->SetEncoding(GetEncoding());
  m_Statements.push_back(pStatementWrapper);
}

// get field
void MysqlPreparedStatement::SetParamInt(int nPosition, int nValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, nValue);
  }
}

void MysqlPreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, dblValue);
  }
}

void MysqlPreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, strValue);
  }
}

void MysqlPreparedStatement::SetParamNull(int nPosition)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition);
  }
}

void MysqlPreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, pData, nDataLength);
  }
}

void MysqlPreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, dateValue);
  }
}

void MysqlPreparedStatement::SetParamBool(int nPosition, bool bValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    m_Statements[nIndex]->SetParam(nPosition, bValue);
  }
}

int MysqlPreparedStatement::GetParameterCount()
{
  MysqlStatementWrapperArray::iterator start = m_Statements.begin();
  MysqlStatementWrapperArray::iterator stop = m_Statements.end();

  int nParameters = 0;
  while (start != stop)
  {
    nParameters += ((MysqlPreparedStatementWrapper*)(*start))->GetParameterCount();
    start++;
  }
  return nParameters;
}

int MysqlPreparedStatement::RunQuery()
{
  MysqlStatementWrapperArray::iterator start = m_Statements.begin();
  MysqlStatementWrapperArray::iterator stop = m_Statements.end();

  int nRows = -1;
  while (start != stop)
  {
    nRows = ((MysqlPreparedStatementWrapper*)(*start))->RunQuery();
    if (((MysqlPreparedStatementWrapper*)(*start))->GetErrorCode() != DATABASE_LAYER_OK)
    {
      SetErrorCode(((MysqlPreparedStatementWrapper*)(*start))->GetErrorCode());
      SetErrorMessage(((MysqlPreparedStatementWrapper*)(*start))->GetErrorMessage());
      ThrowDatabaseException();
      return DATABASE_LAYER_QUERY_RESULT_ERROR;
    }
    start++;
  }
  return nRows;
}

DatabaseResultSet* MysqlPreparedStatement::RunQueryWithResults()
{
  if (m_Statements.size() > 0)
  {
    for (unsigned int i=0; i<(m_Statements.size()-1); i++)
    {
      MysqlPreparedStatementWrapper* pStatement = m_Statements[i];
      pStatement->RunQuery();
      if (pStatement->GetErrorCode() != DATABASE_LAYER_OK)
      {
        SetErrorCode(pStatement->GetErrorCode());
        SetErrorMessage(pStatement->GetErrorMessage());
        ThrowDatabaseException();
        return NULL;
      }
    }

    MysqlPreparedStatementWrapper* pLastStatement = m_Statements[m_Statements.size()-1];
    DatabaseResultSet* pResults = pLastStatement->RunQueryWithResults();
    if (pLastStatement->GetErrorCode() != DATABASE_LAYER_OK)
    {
      SetErrorCode(pLastStatement->GetErrorCode());
      SetErrorMessage(pLastStatement->GetErrorMessage());
      ThrowDatabaseException();
    }
    LogResultSetForCleanup(pResults);
    return pResults;
  }
  else
    return NULL;
}

int MysqlPreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
  if (m_Statements.size() == 0)
    return 0;
    
  // Go through all the elements in the vector
  // Get the number of parameters in each statement
  // Adjust the nPosition for the the broken up statements
  for (unsigned int i=0; i<m_Statements.size(); i++)
  {
    int nParametersInThisStatement = m_Statements[i]->GetParameterCount();

    if (*pPosition > nParametersInThisStatement)
    {
      *pPosition -= nParametersInThisStatement;    // Decrement the position indicator by the number of parameters in this statement
    }
    else
    {
      // We're in the correct statement, return the index
      return i;
    }
  }
  return -1;
}

