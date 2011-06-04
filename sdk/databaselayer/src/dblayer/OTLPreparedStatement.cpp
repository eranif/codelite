#include "OTLPreparedStatement.h"
#include "OTLDatabaseLayer.h"
#include "OTLResultSet.h"

#include <string>

// ctor
OTLPreparedStatement::OTLPreparedStatement(otl_connect* pEnvironment)
{
  m_pEnvironment = pEnvironment;
}

// Maybe we should use the statement string in the parameter
// The otl_stream can be created during the call to RunQuery/RunQueryWithResults
OTLPreparedStatement::OTLPreparedStatement(otl_connect* pEnvironment, otl_stream* pStatement)
{
  m_pEnvironment = pEnvironment;
  AddStatement(pStatement);
}

OTLPreparedStatement::~OTLPreparedStatement()
{
  Close();
}

void OTLPreparedStatement::Close()
{
  try
  {
    CloseResultSets();

    OTLStatementVector::iterator start = m_Statements.begin();
    OTLStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {
      otl_stream *otlstream = *start;
      start++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::AddStatement(otl_stream* pStatement)
{
  m_Statements.push_back(pStatement);
}


// get field
void OTLPreparedStatement::SetParamInt(int nPosition, int nValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //fixme
      //m_Statements[nIndex]->setInt(nPosition, nValue);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //fixme
      //m_Statements[nIndex]->setDouble(nPosition, dblValue);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      wxCharBuffer valueCharBuffer = ConvertToUnicodeStream(strValue);
      std::string stdStrValue(valueCharBuffer);
      //fixme
      //m_Statements[nIndex]->setString(nPosition, stdStrValue);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamNull(int nPosition)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //m_Statements[nIndex]->setNull(nPosition);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //fixme
      //OTL::occi::Blob blob(m_Statements[nIndex]->getConnection());
      //blob.setEmpty();
      //unsigned int nDataCopied = blob.write(nDataLength, (unsigned char*)pData, nDataLength);
      //m_Statements[nIndex]->setBlob(nPosition, blob);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      /*//fixme
           OTL::occi::Date date(m_pEnvironment, dateValue.GetYear(), dateValue.GetMonth(), dateValue.GetDay(), 
               dateValue.GetHour(), dateValue.GetMinute(), dateValue.GetSecond());
           m_Statements[nIndex]->setDate(nPosition, date);
        */
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

void OTLPreparedStatement::SetParamBool(int nPosition, bool bValue)
{
  try
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      //fixme
      //m_Statements[nIndex]->setInt(nPosition, bValue);
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

int OTLPreparedStatement::GetParameterCount()
{
  int nCount = 0;
  try
  {
    OTLStatementVector::iterator start = m_Statements.begin();
    OTLStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {

      /*//fixme
           wxString strSQL = ConvertFromUnicodeStream((*start)->getSQL().c_str());
           nCount += strSQL.Freq('?');
      */
      start++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return 0;
  }
  return nCount;
}


int OTLPreparedStatement::RunQuery()
{
  try
  {
    OTLStatementVector::iterator start = m_Statements.begin();
    OTLStatementVector::iterator stop = m_Statements.end();

    while (start != stop)
    {
      //fixme
      //otl_stream *otlstream = (*start);
      //(*start)->open(50,;
      start++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

DatabaseResultSet* OTLPreparedStatement::RunQueryWithResults()
{
  OTLResultSet* pResultSet = NULL;
  try
  {
    if (m_Statements.size() > 0)
    {
      for (unsigned int i=0; i<(m_Statements.size()-1); i++)
      {
        //fixme
        //m_Statements[i]->execute();
      }

      /*//fixme
         // Deal with the last query separately
         OTL::occi::ResultSet* pOTLResultSet = m_Statements[m_Statements.size()-1]->executeQuery();
         if (pOTLResultSet)
           pResultSet = new OTLResultSet(pOTLResultSet, false);
      */
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
    return NULL;
  }

  if (pResultSet)
    LogResultSetForCleanup(pResultSet);

  return pResultSet;
}

int OTLPreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
  // Don't mess around if there's just one entry in the vector
  if (m_Statements.size() == 0)
    return 0;

  // Go through all the elements in the vector
  // Get the number of parameters in each statement
  // Adjust the nPosition for the the broken up statements
  for (unsigned int i=0; i<m_Statements.size(); i++)
  {
    /*//fixme
       wxString strSQL = ConvertFromUnicodeStream(m_Statements[i]->getSQL().c_str());
       int nParametersInThisStatement = strSQL.Freq('?');
       if (*pPosition > nParametersInThisStatement)
       {
         *pPosition -= nParametersInThisStatement;    // Decrement the position indicator by the number of parameters in this statement
       }
       else
       {
         // We're in the correct statement, return the index
         return i;
       }
    */
  }
  return -1;
}
/*
void OTLPreparedStatement::SetParam(int nPosition, OTLParameter* pParameter)
{
  // First make sure that there are enough elements in the collection
  while (m_Parameters.size() < (unsigned int)(nPosition))
  {
    m_Parameters.push_back(NULL);// otl_null();
  }
  // Free up any data that is being replaced so the allocated memory isn't lost
  if (m_Parameters[nPosition-1] != NULL)
  {
    delete (m_Parameters[nPosition-1]);
  }
  // Now set the new data
  m_Parameters[nPosition-1] = pParameter;
}
*/

