#include "../include/SqliteResultSet.h"
#include "../include/SqlitePreparedStatement.h"
#include "../include/SqliteDatabaseLayer.h"
#include "../include/SqliteResultSetMetaData.h"
#include "../include/DatabaseLayerException.h"
#include "../include/DatabaseErrorCodes.h"

// ctor
SqliteResultSet::SqliteResultSet()
 : DatabaseResultSet()
{
  m_pStatement = NULL;
  m_pSqliteStatement = NULL;
  m_bManageStatement = false;
}

SqliteResultSet::SqliteResultSet(SqlitePreparedStatement* pStatement, bool bManageStatement /*= false*/)
 : DatabaseResultSet()
{
  m_pStatement = pStatement;
  m_pSqliteStatement = m_pStatement->GetLastStatement();
  m_bManageStatement = bManageStatement;

  // Populate field lookup map
  int nFieldCount = sqlite3_column_count(m_pSqliteStatement);
  for (int i=0; i<nFieldCount; i++)
  {
    wxString strField = ConvertFromUnicodeStream(sqlite3_column_name(m_pSqliteStatement, i));
    m_FieldLookupMap[strField] = i;
  }
}

// dtor
SqliteResultSet::~SqliteResultSet()
{
  Close();
}


void SqliteResultSet::Close()
{
  CloseMetaData();

  if (m_bManageStatement)
  {
    if (m_pStatement != NULL)
    {
      m_pStatement->Close();
      wxDELETE(m_pStatement);
    }
  }
}


bool SqliteResultSet::Next()
{
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  int nReturn = sqlite3_step(m_pSqliteStatement);
 
  if (nReturn != SQLITE_ROW)
    sqlite3_reset(m_pSqliteStatement);

  if ((nReturn != SQLITE_ROW) && (nReturn != SQLITE_DONE))
  {
    wxLogError(_("Error with RunQueryWithResults\n"));
    SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
#if SQLITE_VERSION_NUMBER>=3002002
    // sqlite3_db_handle wasn't added to the SQLite3 API until version 3.2.2
    SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(sqlite3_db_handle(m_pSqliteStatement))));
#else
    SetErrorMessage(_("Unknown error advancing result set"));
#endif
    ThrowDatabaseException();
    return false;
  }
  
  return (nReturn == SQLITE_ROW);
}


// get field
int SqliteResultSet::GetResultInt(int nField)
{
  int nValue = -1;
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  nValue = sqlite3_column_int(m_pSqliteStatement, nField-1);

  return nValue;
}

wxString SqliteResultSet::GetResultString(int nField)
{
  wxString strValue = _("");
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  strValue = ConvertFromUnicodeStream((const char*)(sqlite3_column_text(m_pSqliteStatement, nField-1)));

  return strValue;
}

long SqliteResultSet::GetResultLong(int nField)
{
  long nValue = -1;
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  nValue = sqlite3_column_int(m_pSqliteStatement, nField-1);

  return nValue;
}

bool SqliteResultSet::GetResultBool(int nField)
{
  int nValue = 0;
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  nValue = sqlite3_column_int(m_pSqliteStatement, nField-1);

  return (nValue != 0);
}

wxDateTime SqliteResultSet::GetResultDate(int nField)
{
  // Don't use nField-1 here since GetResultString will take care of that
  wxString strDate = GetResultString(nField);
  wxDateTime date;
  // First check for the 2-digit year format
  if (date.ParseFormat(strDate, _("%m/%d/%y %H:%M:%S")) != NULL)
  {
    return date;
  }
  else if (date.ParseDateTime(strDate) != NULL)
  {
    return date;
  }
  else if (date.ParseDate(strDate) != NULL)
  {
    return date;
  }
  else
  {
    return wxInvalidDateTime;
  }
}

double SqliteResultSet::GetResultDouble(int nField)
{
  double dblValue = -1;
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  dblValue = sqlite3_column_double(m_pSqliteStatement, nField-1);

  return dblValue;
}

void* SqliteResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  int nLength = 0;
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  nLength = sqlite3_column_bytes(m_pSqliteStatement, nField-1);
  if (nLength < 1)
  {
    wxMemoryBuffer tempBuffer(0);
    tempBuffer.SetDataLen(0);
    tempBuffer.SetBufSize(0);
    Buffer = tempBuffer;
  
    return NULL;
  }

  const void* pBlob = sqlite3_column_blob(m_pSqliteStatement, nField-1);

  wxMemoryBuffer tempBuffer(nLength);
  void* pBuffer = tempBuffer.GetWriteBuf(nLength);
  memcpy(pBuffer, pBlob, nLength);
  tempBuffer.UngetWriteBuf(nLength);
  tempBuffer.SetDataLen(nLength);
  tempBuffer.SetBufSize(nLength);

  Buffer = tempBuffer;
  
  return Buffer.GetData();
}

bool SqliteResultSet::IsFieldNull(int nField)
{
  if (m_pSqliteStatement == NULL)
    m_pSqliteStatement = m_pStatement->GetLastStatement();
  return (NULL == sqlite3_column_text(m_pSqliteStatement, nField-1));
}

int SqliteResultSet::LookupField(const wxString& strField)
{
  StringToIntMap::iterator SearchIterator = m_FieldLookupMap.find(strField);
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
    return ((*SearchIterator).second + 1);  // Add +1 to make the result set 1-based rather than 0-based
  }
}

ResultSetMetaData* SqliteResultSet::GetMetaData()
{
  ResultSetMetaData* pMetaData = new SqliteResultSetMetaData(m_pSqliteStatement);
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}

