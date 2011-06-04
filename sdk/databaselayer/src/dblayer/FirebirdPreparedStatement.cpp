#include "../include/FirebirdPreparedStatement.h"
#include "../include/FirebirdDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

// ctor()
FirebirdPreparedStatement::FirebirdPreparedStatement(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction)
 : PreparedStatement()
{
  m_pInterface = pInterface;
  m_bManageTransaction = false;
  m_pTransaction = pTransaction;
  m_pDatabase = pDatabase;
}
/*
FirebirdPreparedStatement::FirebirdPreparedStatement(FirebirdPreparedStatementWrapper Statement)
{
  SetErrorCode(DATABASE_LAYER_OK);
  SetErrorMessage(_(""));
  
  m_Statements.push_back(Statement);
}
*/
FirebirdPreparedStatement::~FirebirdPreparedStatement()
{
  Close();
}

void FirebirdPreparedStatement::Close()
{
  CloseResultSets();

  FirebirdStatementVector::iterator start = m_Statements.begin();
  FirebirdStatementVector::iterator stop = m_Statements.end();

  while (start != stop)
  {
    FirebirdPreparedStatementWrapper* pWrapper = (FirebirdPreparedStatementWrapper*)(*start);
    wxDELETE(pWrapper);
    (*start) = NULL;
    start++;
  }

  // Close the transaction if we're in charge of it
  if (m_bManageTransaction && m_pTransaction)
  {
    int nReturn = m_pInterface->GetIscCommitTransaction()(m_Status, &m_pTransaction);
    m_pTransaction = NULL;
    if (nReturn != 0)
    {
      InterpretErrorCodes();
      ThrowDatabaseException();
    }
  }
}
  
void FirebirdPreparedStatement::AddPreparedStatement(const wxString& strSQL)
{
  FirebirdPreparedStatementWrapper* pWrapper = new FirebirdPreparedStatementWrapper(m_pInterface, m_pDatabase, m_pTransaction, strSQL);
  pWrapper->SetEncoding(GetEncoding());
  m_Statements.push_back(pWrapper);
}

FirebirdPreparedStatement* FirebirdPreparedStatement::CreateStatement(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction, const wxString& strSQL, const wxCSConv* conv)
{
  wxArrayString Queries = ParseQueries(strSQL);

  wxArrayString::iterator start = Queries.begin();
  wxArrayString::iterator stop = Queries.end();

  FirebirdPreparedStatement *pStatement = NULL;

  if (Queries.size() < 1)
  {
    pStatement = new FirebirdPreparedStatement(pInterface, pDatabase, pTransaction);
    pStatement->SetEncoding(conv);

    pStatement->SetErrorCode(DATABASE_LAYER_ERROR);
    pStatement->SetErrorMessage(_("No SQL Statements found"));

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    // If we're using exceptions, then assume that the calling program won't
    //  won't get the pStatement pointer back.  So delete is now before
    //  throwing the exception
    DatabaseLayerException error(pStatement->GetErrorCode(), pStatement->GetErrorMessage());
    try
    {
      delete pStatement; //It's probably better to manually iterate over the list and close the statements, but for now just let close do it
    }
    catch (DatabaseLayerException& e)
    {
    }

    throw error;
#endif
    return NULL;
  }
  
  
  // Start a new transaction if appropriate
  if (pTransaction == NULL)
  {
    pTransaction = 0L;
    ISC_STATUS_ARRAY status;
    int nReturn = pInterface->GetIscStartTransaction()(status, &pTransaction, 1, &pDatabase, 0 /*tpb_length*/, NULL/*tpb*/);
    pStatement = new FirebirdPreparedStatement(pInterface, pDatabase, pTransaction);
    pStatement->SetEncoding(conv);
    if (nReturn != 0)
    {
      long nSqlCode = pInterface->GetIscSqlcode()(status);
      pStatement->SetErrorCode(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode));
      pStatement->SetErrorMessage(FirebirdDatabaseLayer::TranslateErrorCodeToString(pInterface, nSqlCode, status));

#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
      // If we're using exceptions, then assume that the calling program won't
      //  won't get the pStatement pointer back.  So delete it now before
      //  throwing the exception
      try
      {
        delete pStatement; //It's probably better to manually iterate over the list and close the statements, but for now just let close do it
      }
      catch (DatabaseLayerException& e)
      {
      }

      DatabaseLayerException error(pStatement->GetErrorCode(), pStatement->GetErrorMessage());
      throw error;
#endif
      return pStatement;
    }
    
    pStatement->SetManageTransaction(true);
  }
  else
  {
    pStatement = new FirebirdPreparedStatement(pInterface, pDatabase, pTransaction);
    pStatement->SetEncoding(conv);
    pStatement->SetManageTransaction(false);
  }

  while (start != stop)
  {
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    try
    {
#endif
    pStatement->AddPreparedStatement((*start));
    }
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    catch (DatabaseLayerException& e)
    {
      try
      {
        delete pStatement; //It's probably better to manually iterate over the list and close the statements, but for now just let close do it
      }
      catch (DatabaseLayerException& e)
      {
      }

      // Pass on the error
      throw e;
    }
#endif

    if (pStatement->GetErrorCode() != DATABASE_LAYER_OK)
    {
      // If we're using exceptions, then assume that the calling program won't
      //  won't get the pStatement pointer back.  So delete is now before
      //  throwing the exception
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
      // Set the error code and message
      DatabaseLayerException error(pStatement->GetErrorCode(), pStatement->GetErrorMessage());
 
      try
      {
        delete pStatement; //It's probably better to manually iterate over the list and close the statements, but for now just let close do it
      }
      catch (DatabaseLayerException& e)
      {
      }

      // Pass on the error
      throw error;
#endif

      return pStatement;
    }
    start++;
  }
    
  // Success?  Return the statement
  return pStatement;
}

// get field
void FirebirdPreparedStatement::SetParamInt(int nPosition, int nValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, nValue);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, dblValue);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, strValue);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamNull(int nPosition)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, pData, nDataLength);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, dateValue);
  else
    SetInvalidParameterPositionError(nPosition);
}

void FirebirdPreparedStatement::SetParamBool(int nPosition, bool bValue)
{
  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
    m_Statements[nIndex]->SetParam(nPosition, bValue);
  else
    SetInvalidParameterPositionError(nPosition);
}

int FirebirdPreparedStatement::GetParameterCount()
{
  FirebirdStatementVector::iterator start = m_Statements.begin();
  FirebirdStatementVector::iterator stop = m_Statements.end();

  int nParameters = 0;
  while (start != stop)
  {
    nParameters += ((FirebirdPreparedStatementWrapper*)(*start))->GetParameterCount();
    start++;
  }
  return nParameters;
}
  
int FirebirdPreparedStatement::RunQuery()
{
  FirebirdStatementVector::iterator start = m_Statements.begin();
  FirebirdStatementVector::iterator stop = m_Statements.end();

  int nRows = -1;
  while (start != stop)
  {
    nRows = ((FirebirdPreparedStatementWrapper*)(*start))->RunQuery();
    if (((FirebirdPreparedStatementWrapper*)(*start))->GetErrorCode() != DATABASE_LAYER_OK)
    {
      SetErrorCode(((FirebirdPreparedStatementWrapper*)(*start))->GetErrorCode());
      SetErrorMessage(((FirebirdPreparedStatementWrapper*)(*start))->GetErrorMessage());
      return DATABASE_LAYER_QUERY_RESULT_ERROR;
    }
    
    start++;
  }

  // If the statement is managing the transaction then commit it now
  if (m_bManageTransaction)
  {
    int nReturn = m_pInterface->GetIscCommitRetaining()(m_Status, &m_pTransaction);
    //int nReturn = isc_commit_transaction(m_Status, &m_pTransaction);
    // We're done with the transaction, so set it to NULL so that we know that a new transaction must be started if we run any queries
    if (nReturn != 0)
    {
      InterpretErrorCodes();
      ThrowDatabaseException();
    }
  }

  return nRows;
}

DatabaseResultSet* FirebirdPreparedStatement::RunQueryWithResults()
{
  if (m_Statements.size() > 0)
  {
    // Assume that only the last statement in the array returns the result set
    for (unsigned int i=0; i<m_Statements.size()-1; i++)
    {
      m_Statements[i]->RunQuery();
      if (m_Statements[i]->GetErrorCode() != DATABASE_LAYER_OK)
      {
        SetErrorCode(m_Statements[i]->GetErrorCode());
        SetErrorMessage(m_Statements[i]->GetErrorMessage());
        return NULL;
      }
    }

    FirebirdPreparedStatementWrapper* pLastStatement = m_Statements[m_Statements.size()-1];
    // If the statement is managing the transaction then commit it now
    if (m_bManageTransaction)
    {
      //int nReturn = isc_commit_retaining(m_Status, &m_pTransaction);
      int nReturn = m_pInterface->GetIscCommitTransaction()(m_Status, &m_pTransaction);
      // We're done with the transaction, so set it to NULL so that we know that a new transaction must be started if we run any queries
      if (nReturn != 0)
      {
        InterpretErrorCodes();
        ThrowDatabaseException();
      }
      
      // Start a new transaction
      nReturn = m_pInterface->GetIscStartTransaction()(m_Status, &m_pTransaction, 1, &m_pDatabase, 0 /*tpb_length*/, NULL/*tpb*/);
      if (nReturn != 0)
      {
        InterpretErrorCodes();
        ThrowDatabaseException();
        return NULL;
      }

      // Make sure to update the last statements pointer to the transaction
      pLastStatement->SetTransaction(m_pTransaction);
    }

    // The result set will be in charge of the result set now so change flag so that we don't try to close the transaction when the statement closes
    //m_bManageTransaction = false;

    DatabaseResultSet* pResultSet = pLastStatement->RunQueryWithResults();
    if (pResultSet)
      pResultSet->SetEncoding(GetEncoding());
    if (pLastStatement->GetErrorCode() != DATABASE_LAYER_OK)
    {
      SetErrorCode(pLastStatement->GetErrorCode());
      SetErrorMessage(pLastStatement->GetErrorMessage());
      
      // Wrap the result set deletion in try/catch block if using exceptions.
      //We want to make sure the original error gets to the user
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
      try
      {
#endif
      if (pResultSet)
        delete pResultSet;
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
      }
      catch (DatabaseLayerException& e)
      {
      }
#endif

      return NULL;
    }

    LogResultSetForCleanup(pResultSet);
    return pResultSet;
  }
  else
    return NULL;
}

int FirebirdPreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
  // Don't mess around if there's just one entry in the vector
  if (m_Statements.size() <= 1)
    return 0;
    
  // Go through all the elements in the vector
  // Get the number of parameters in each statement
  // Adjust the nPosition for the the broken up statements
  for (unsigned int i=0; i<m_Statements.size(); i++)
  {
    int nParametersInThisStatement = 0;
    nParametersInThisStatement = m_Statements[i]->GetParameterCount();
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

void FirebirdPreparedStatement::SetInvalidParameterPositionError(int nPosition)
{
  SetErrorCode(DATABASE_LAYER_ERROR);
  SetErrorMessage(_("Invalid Prepared Statement Parameter"));

  ThrowDatabaseException();
}

void FirebirdPreparedStatement::InterpretErrorCodes()
{
  wxLogError(_("FirebirdPreparesStatement::InterpretErrorCodes()\n"));

  long nSqlCode = m_pInterface->GetIscSqlcode()(m_Status);
  SetErrorCode(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode));
  SetErrorMessage(FirebirdDatabaseLayer::TranslateErrorCodeToString(m_pInterface, nSqlCode, m_Status));
}

