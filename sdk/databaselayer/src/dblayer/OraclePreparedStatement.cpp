#include "../include/OraclePreparedStatement.h"
#include "../include/OracleDatabaseLayer.h"
#include "../include/OracleResultSet.h"

extern "C" {
#include <oci.h>
}

#include <string>

// ctor
OraclePreparedStatement::OraclePreparedStatement(oracle::occi::Environment* pEnvironment)
{
  m_pEnvironment = pEnvironment;
}
/*
OraclePreparedStatement::OraclePreparedStatement(oracle::occi::Environment* pEnvironment, oracle::occi::Statement* pStatement)
{
  m_pEnvironment = pEnvironment;
  AddStatement(pStatement);
}
*/
OraclePreparedStatement::~OraclePreparedStatement()
{
  Close();
}
  
void OraclePreparedStatement::Close()
{
  CloseResultSets();

  try
  {
    OracleStatementVector::iterator start = m_Statements.begin();
    OracleStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {
      /*
      oracle::occi::Connection* pDatabase = (*start)->getConnection();
      if (pDatabase)
        pDatabase->terminateStatement((*start));
      */
      delete (*start);
      start++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}
/*
void OraclePreparedStatement::AddStatement(oracle::occi::Statement* pStatement)
{
  m_Statements.push_back(pStatement);
}
*/
void OraclePreparedStatement::AddStatement(OraclePreparedStatementWrapper* pStatement)
{
  m_Statements.push_back(pStatement);
}

// get field
void OraclePreparedStatement::SetParamInt(int nPosition, int nValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      m_Statements[nIndex]->GetStatement()->setInt(nPosition, nValue);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      m_Statements[nIndex]->GetStatement()->setDouble(nPosition, dblValue);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      wxCharBuffer valueCharBuffer = ConvertToUnicodeStream(strValue);
      std::string stdStrValue(valueCharBuffer);
      m_Statements[nIndex]->GetStatement()->setString(nPosition, stdStrValue);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamNull(int nPosition)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      oracle::occi::Statement* pStatement = m_Statements[nIndex]->GetStatement();
      oracle::occi::PObject* pObject = pStatement->getObject(nPosition);
      pObject->setNull();
      pStatement->setObject(nPosition, pObject);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //wxMemoryBuffer buffer);
      //memcpy(pData, buffer.GetWriteBuf(nDataLength), nDataLength);
      //m_Statements[nIndex]->SetBlob(nPosition, buffer);
      OCIStmt* pOciStmt = m_Statements[nIndex]->GetStatement()->getOCIStatement();
      OCIBind* bindhp = NULL;
      OCIError* errhp = NULL;
      sword _errno = OCIBindByPos(pOciStmt, &bindhp, errhp, 1, (dvoid*)pData, nDataLength,
        SQLT_LNG, 0, 0, 0, 0, 0, OCI_DEFAULT);

      //oracle::occi::Blob blob(m_Statements[nIndex]->GetStatement()->getConnection());
      //blob.setEmpty();
      //unsigned int nDataCopied = blob.write(nDataLength, (unsigned char*)pData, nDataLength);
      //m_Statements[nIndex]->GetStatement()->setBlob(nPosition, blob);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      oracle::occi::Date date(m_pEnvironment, dateValue.GetYear(), dateValue.GetMonth()+1, dateValue.GetDay(), 
          dateValue.GetHour(), dateValue.GetMinute(), dateValue.GetSecond());
      m_Statements[nIndex]->GetStatement()->setDate(nPosition, date);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

void OraclePreparedStatement::SetParamBool(int nPosition, bool bValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      m_Statements[nIndex]->GetStatement()->setInt(nPosition, bValue);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

int OraclePreparedStatement::GetParameterCount()
{
  int nCount = 0;
  try
  {
    OracleStatementVector::iterator start = m_Statements.begin();
    OracleStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {
      //wxString strSQL = ConvertFromUnicodeStream((*start)->getSQL().c_str());
      //nCount += strSQL.Freq(':');//strSQL.Freq('?');
      nCount += (*start)->GetParameterCount();

      start++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return 0;
  }
  return nCount;
}


void OraclePreparedStatement::RunQuery()
{
  try
  {
    OracleStatementVector::iterator start = m_Statements.begin();
    OracleStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {
      (*start)->GetStatement()->execute();
      start++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

DatabaseResultSet* OraclePreparedStatement::RunQueryWithResults()
{
  OracleResultSet* pResultSet = NULL;
  try
  {
    if (m_Statements.size() > 0)
    {
      for (unsigned int i=0; i<(m_Statements.size()-1); i++)
      {
        m_Statements[i]->GetStatement()->execute();
      }

      // Deal with the last query separately
      oracle::occi::ResultSet* pOracleResultSet = m_Statements[m_Statements.size()-1]->GetStatement()->executeQuery();
      if (pOracleResultSet)
        pResultSet = new OracleResultSet(pOracleResultSet, false);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
    return NULL;
  }
  return pResultSet;
}

int OraclePreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
  // Don't mess around if there's just one entry in the vector
  if (m_Statements.size() == 0)
    return 0;
    
  // Go through all the elements in the vector
  // Get the number of parameters in each statement
  // Adjust the nPosition for the the broken up statements
  for (unsigned int i=0; i<m_Statements.size(); i++)
  {
    //wxString strSQL = ConvertFromUnicodeStream(m_Statements[i]->getSQL().c_str());
    //int nParametersInThisStatement = strSQL.Freq('?');
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


