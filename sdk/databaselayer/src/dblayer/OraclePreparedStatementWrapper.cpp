#include "../include/OraclePreparedStatementWrapper.h"

// ctor
OraclePreparedStatementWrapper::OraclePreparedStatementWrapper(oracle::occi::Connection* m_pDatabase, const wxString& strSQL)
{
  wxCharBuffer sqlBuffer = ConvertToUnicodeStream(TranslateSQL(strSQL));
  std::string strStdSQL(sqlBuffer);
  m_pStatement = m_pDatabase->createStatement(strStdSQL);
}

// dtor
OraclePreparedStatementWrapper::~OraclePreparedStatementWrapper()
{
  if (m_pStatement)
  {
    oracle::occi::Connection* pDatabase = m_pStatement->getConnection();
    if (pDatabase)
      pDatabase->terminateStatement(m_pStatement);
    delete m_pStatement;
  }
}

wxString OraclePreparedStatementWrapper::TranslateSQL(const wxString& strOriginalSQL)
{
  int nParameterIndex = 1;
  wxString strReturn = strOriginalSQL;
  if (strReturn.Last() == ';')
    strReturn.RemoveLast();
  int nFound = strReturn.Replace(_("?"), wxString::Format(_(":%d"), nParameterIndex), false);
  if (nFound == 0)
  {
    m_nParameterCount = 0;
  }
  else
  {
    while (nFound != 0)
    {
      nParameterIndex++;
      nFound = strReturn.Replace(_("?"), wxString::Format(_(":%d"), nParameterIndex), false);
    }
    m_nParameterCount = nParameterIndex-1;
  }
  return strReturn;
}


