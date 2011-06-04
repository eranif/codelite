#include "../include/OracleResultSet.h"
#include "../include/OracleResultSetMetaData.h"
#include "../include/OracleDatabaseLayer.h"
#include "../include/DatabaseLayerException.h"
#include "../include/DatabaseErrorCodes.h"

  // ctor
OracleResultSet::OracleResultSet(oracle::occi::ResultSet* pResultSet, bool bManageStatement /*= false*/)
{
  try
  {
    m_pResultSet = pResultSet;
    m_bManageStatement = bManageStatement;

    std::vector<oracle::occi::MetaData> m_MetaData = m_pResultSet->getColumnListMetaData();
    std::vector<oracle::occi::MetaData>::iterator start = m_MetaData.begin();
    std::vector<oracle::occi::MetaData>::iterator stop = m_MetaData.end();
    int nCounter = 1;
    while (start != stop)
    {
      wxString strField = ConvertFromUnicodeStream((*start).getString(oracle::occi::MetaData::ATTR_NAME).c_str());
      m_FieldLookupMap[strField.Upper()] = nCounter;
      start++;
      nCounter++;
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}

// dtor
OracleResultSet::~OracleResultSet()
{
  Close();
}

bool OracleResultSet::Next()
{
  try
  {
    return (m_pResultSet->next());
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return false;
}

void OracleResultSet::Close()
{
  CloseMetaData();

  try
  {
    oracle::occi::Statement* pStatement = m_pResultSet->getStatement();

    if (pStatement)
    {
      pStatement->closeResultSet(m_pResultSet);

      if (m_bManageStatement)
      {
        oracle::occi::Connection* pDatabase = pStatement->getConnection();
        if (pDatabase)
          pDatabase->terminateStatement(pStatement);
      }
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
}
  
// get field
int OracleResultSet::GetResultInt(int nField)
{
  int nValue = -1;
  try
  {
    nValue = m_pResultSet->getInt(nField);
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return nValue;
}

wxString OracleResultSet::GetResultString(int nField)
{
  wxString strValue = wxEmptyString;
  try
  {
    strValue = ConvertFromUnicodeStream(m_pResultSet->getString(nField).c_str());
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return strValue;
}

long OracleResultSet::GetResultLong(int nField)
{
  long lValue = -1L;
  try
  {
    lValue = m_pResultSet->getInt(nField);
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return lValue;
}

bool OracleResultSet::GetResultBool(int nField)
{
  bool bValue = false;
  try
  {
    bValue = (m_pResultSet->getInt(nField) != 0);
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return bValue;
}

wxDateTime OracleResultSet::GetResultDate(int nField)
{
  wxDateTime date = wxInvalidDateTime;
  try
  {
    //int nType = m_MetaData[nField-1].getInt(MetaData::ATTR_DATA_TYPE);
    //if (nType = OCCI_SQLT_DAT)
    //{
    //}
    if (!m_pResultSet->isNull(nField))
    {
      oracle::occi::Timestamp ts = m_pResultSet->getTimestamp(nField);
      if (!ts.isNull())
      {
        std::string stringDate = ts.toText("mm/dd/yyyy hh24:mi:ss", 0);
        wxString stringDateWx(stringDate.c_str());
        wxPrintf("Converting date: %s\n", stringDateWx.c_str());
        if (date.ParseFormat(stringDateWx, _("%m/%d/%y %H:%M:%S")) == NULL)
        {
          // If the first conversion didn't work, try again
          date.ParseDateTime(stringDateWx);
        }
      }
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return date;
}

void* OracleResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  try
  {
    if (m_pResultSet->isNull(nField))
    {
      return NULL;
    }
    else
    {
      oracle::occi::Blob blob = m_pResultSet->getBlob(nField);
      blob.open(oracle::occi::OCCI_LOB_READONLY);
      int nLength = blob.length();
      unsigned char* pBuffer = (unsigned char*)(Buffer.GetWriteBuf(nLength));
      int nLengthRead = blob.read(nLength, pBuffer, nLength);
    }
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return Buffer.GetData();
}

double OracleResultSet::GetResultDouble(int nField)
{
  double dblValue = -1;
  try
  {
    dblValue = m_pResultSet->getDouble(nField);
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return dblValue;
}

bool OracleResultSet::IsFieldNull(int nField)
{
  try
  {
    return m_pResultSet->isNull(nField);
  }
  catch (oracle::occi::SQLException& e)
  {
    SetErrorCode(OracleDatabaseLayer::TranslateErrorCode(e.getErrorCode()));
    SetErrorMessage(ConvertFromUnicodeStream(e.getMessage().c_str()));
    ThrowDatabaseException();
  }
  return true;
}

int OracleResultSet::LookupField(const wxString& strField)
{
  StringToIntMap::iterator SearchIterator = m_FieldLookupMap.find(strField.Upper());
  if (SearchIterator == m_FieldLookupMap.end())
  {
    wxString msg(_("Field '") + strField + _("' not found in the resultset"));
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    DatabaseLayerException error(DATABASE_LAYER_FIELD_NOT_IN_RESULTSET, msg);
    throw error;
#else
    wxLogError(msg);
#endif
    return -1;
  }
  else
  {
    return (*SearchIterator).second;
  }
}

ResultSetMetaData* OracleResultSet::GetMetaData()
{
  return new OracleResultSetMetaData(m_MetaData);
}

