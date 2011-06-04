#include "../include/MysqlPreparedStatementResultSet.h"
#include "../include/MysqlResultSetMetaData.h"
#include "../include/MysqlDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

MysqlPreparedStatementResultSet::MysqlPreparedStatementResultSet(MysqlInterface* pInterface)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pStatement = NULL;
  m_pResultBindings = NULL;
  m_bManageStatement = false;
}

MysqlPreparedStatementResultSet::MysqlPreparedStatementResultSet(MysqlInterface* pInterface, MYSQL_STMT* pStatement, bool bManageStatement /* = false*/)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pStatement = pStatement;
  m_pResultBindings = NULL;
  m_bManageStatement = bManageStatement;
  MYSQL_RES* pResultMetadata = m_pInterface->GetMysqlStmtResultMetadata()(m_pStatement);
  if (!pResultMetadata)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
    ThrowDatabaseException();
  }
  else
  {
    int nParameters = m_pInterface->GetMysqlNumFields()(pResultMetadata);
    m_pResultBindings = new MYSQL_BIND[nParameters];
    memset(m_pResultBindings, 0, sizeof(MYSQL_BIND)*nParameters);

    MYSQL_BIND* pCurrentBinding = m_pResultBindings;
    MYSQL_FIELD* pCurrentField = pResultMetadata->fields;
    for (int i=0; i<nParameters; i++)
    {
      // Set up the map so we can look this value up later
      wxString strFieldName = ConvertFromUnicodeStream(pCurrentField->name);

      MysqlPreparedStatementParameter* pParameter = new MysqlPreparedStatementParameter(pCurrentBinding, pCurrentField);
      if (pParameter)
        pParameter->SetEncoding(GetEncoding());
      
      m_BindingWrappers[i] = pParameter;
      m_FieldLookupMap[strFieldName] = i;

      // Move to the next fields
      pCurrentBinding++;
      pCurrentField++;
    }
    m_pInterface->GetMysqlStmtBindResult()(m_pStatement, m_pResultBindings);
    m_pInterface->GetMysqlFreeResult()(pResultMetadata);
  }
}

MysqlPreparedStatementResultSet::~MysqlPreparedStatementResultSet()
{
  Close();
}

bool MysqlPreparedStatementResultSet::Next()
{
  ClearPreviousData();
  return (m_pInterface->GetMysqlStmtFetch()(m_pStatement) != MYSQL_NO_DATA);
}

void MysqlPreparedStatementResultSet::Close()
{
  ResetErrorCodes();

  CloseMetaData();

  MYSQL_RES* pResultMetadata = m_pInterface->GetMysqlStmtResultMetadata()(m_pStatement);
  if (!pResultMetadata)
  {
    SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(m_pInterface->GetMysqlStmtErrno()(m_pStatement)));
    SetErrorMessage(ConvertFromUnicodeStream(m_pInterface->GetMysqlStmtError()(m_pStatement)));
    ThrowDatabaseException();
  }
  else
  {
    int nParameters = m_pInterface->GetMysqlNumFields()(pResultMetadata);
    for (int i=0; i<nParameters; i++)
    {
//      int nType = m_pResultBindings[i].buffer_type;
//      if (nType == MYSQL_TYPE_STRING || nType == MYSQL_TYPE_VAR_STRING || nType == MYSQL_TYPE_BLOB
//          || nType == MYSQL_TYPE_TINY_BLOB || nType == MYSQL_TYPE_MEDIUM_BLOB || nType == MYSQL_TYPE_LONG_BLOB)
//      {
//      void* pData = m_pResultBindings[i].buffer;
//      if (pData != NULL)
//      {
//        free(m_pResultBindings[i].buffer);
//        m_pResultBindings[i].buffer = NULL;
//      }
//      }
    }
    m_pInterface->GetMysqlFreeResult()(pResultMetadata);
  }

  IntToMysqlParameterMap::iterator start = m_BindingWrappers.begin();
  IntToMysqlParameterMap::iterator stop = m_BindingWrappers.end();

  while (start != stop)
  {
    wxDELETE((*start).second);
    start++;
  }
  m_BindingWrappers.clear();

  wxDELETEA(m_pResultBindings);

  if (m_pStatement != NULL)
  {
    m_pInterface->GetMysqlStmtFreeResult()(m_pStatement);
    if (m_bManageStatement)
      m_pInterface->GetMysqlStmtClose()(m_pStatement);
    m_pStatement = NULL;
  }
}

// get field
int MysqlPreparedStatementResultSet::GetResultInt(int nField)
{
  return GetResultLong(nField);
}

wxString MysqlPreparedStatementResultSet::GetResultString(int nField)
{
  wxString strValue = _("");
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
    {
      strValue = ConvertFromUnicodeStream((char*)(pResultBinding->buffer));
    }
  }
  return strValue;
}

long MysqlPreparedStatementResultSet::GetResultLong(int nField)
{
  long nValue = 0;
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
    {
      int nType = pResultBinding->buffer_type;/*
      if (nType == MYSQL_TYPE_LONGLONG)
      {
          nValue = *((my_ulonglong*)(pResultBinding->buffer));
      }
      else
      {
        wxString strValue = (char*)(pResultBinding->buffer);
        strValue.ToLong(&nValue);
      }*/

      switch (nType)
      {
        case MYSQL_TYPE_TINY:
          nValue = *((char*)(pResultBinding->buffer));
          break;
        case MYSQL_TYPE_SHORT:
          nValue = *((short int*)(pResultBinding->buffer));
          break;
        case MYSQL_TYPE_LONG:
          nValue = *((long*)(pResultBinding->buffer));
          break;
        case MYSQL_TYPE_LONGLONG:
          nValue = *((my_ulonglong*)(pResultBinding->buffer));
          break;
        default:
          break;
      };
      /**/
    }
  }
  return nValue;
}

bool MysqlPreparedStatementResultSet::GetResultBool(int nField)
{
  bool bValue = false;
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
      bValue = (*((int*)(pResultBinding->buffer)) != 0);
  }
  return bValue;
}

wxDateTime MysqlPreparedStatementResultSet::GetResultDate(int nField)
{
  wxDateTime returnDate = wxInvalidDateTime;
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
    {
      MYSQL_TIME* pDate = (MYSQL_TIME*)(pResultBinding->buffer);
      returnDate.Set(pDate->day, wxDateTime::Month(pDate->month-1), pDate->year, pDate->hour, pDate->minute, pDate->second);
    }
  }
  return returnDate;
}

void* MysqlPreparedStatementResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  void* pReturn = NULL;
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
    {
      unsigned long nBufferLength = 0;
      if (pResultBinding->length)
        nBufferLength = (*pResultBinding->length);
      else
        nBufferLength = pResultBinding->buffer_length;

      wxMemoryBuffer tempBuffer(nBufferLength);
      void* pBuffer = tempBuffer.GetWriteBuf(nBufferLength);
      memcpy(pBuffer, pResultBinding->buffer, nBufferLength);
      tempBuffer.UngetWriteBuf(nBufferLength);
      tempBuffer.SetDataLen(nBufferLength);
      tempBuffer.SetBufSize(nBufferLength);
      Buffer = tempBuffer;

      pReturn = Buffer.GetData();
    }
    else
    {
      wxMemoryBuffer tempBuffer(0);
      tempBuffer.SetDataLen(0);
      tempBuffer.SetBufSize(0);
      Buffer = tempBuffer;
    }
  }
  else
  {
    wxMemoryBuffer tempBuffer(0);
    tempBuffer.SetDataLen(0);
    tempBuffer.SetBufSize(0);
    Buffer = tempBuffer;
  }

  return pReturn;
}

double MysqlPreparedStatementResultSet::GetResultDouble(int nField)
{
  double dblValue = 0.0;
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  if (pResultBinding != NULL)
  {
    if ((*(pResultBinding->is_null) == false))
    {
      int nType = pResultBinding->buffer_type;
      switch (nType)
      {
        case MYSQL_TYPE_FLOAT:
          dblValue = *((float*)(pResultBinding->buffer));
          break;
        case MYSQL_TYPE_DOUBLE:
          dblValue = *((double*)(pResultBinding->buffer));
          break;
        default:
          break;
      };
    }
  }
  return dblValue;
}

bool MysqlPreparedStatementResultSet::IsFieldNull(int nField)
{
  MYSQL_BIND* pResultBinding = GetResultBinding(nField);
  my_bool isNull = *(pResultBinding->is_null);
  if (isNull)
    return true;
  else
    return false;
}

int MysqlPreparedStatementResultSet::LookupField(const wxString& strField)
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
    return -1;
#endif
  }
  else
  {
    return ((*SearchIterator).second+1); // Add +1 to make the result set 1-based rather than 0-based
  }
}

MYSQL_BIND* MysqlPreparedStatementResultSet::GetResultBinding(int nField)
{
  IntToMysqlParameterMap::iterator finder = m_BindingWrappers.find(nField-1);

  if (finder == m_BindingWrappers.end())
  {
    wxString msg(_("Field '") + wxString::Format(_("%d"), nField) + _("' not found in the resultset"));
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    DatabaseLayerException error(DATABASE_LAYER_FIELD_NOT_IN_RESULTSET, msg);
    throw error;
#else
    wxLogError(msg);
    return NULL;
#endif
  }
  else
  {
    return (*finder).second->GetMysqlBind();
  }
}

ResultSetMetaData* MysqlPreparedStatementResultSet::GetMetaData()
{
  ResultSetMetaData* pMetaData = new MysqlResultSetMetaData(m_pInterface, m_pInterface->GetMysqlStmtResultMetadata()(m_pStatement));
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}

void MysqlPreparedStatementResultSet::ClearPreviousData()
{
  // Go through all the bindings and clear the data resetting for the next retrieval
  IntToMysqlParameterMap::iterator start = m_BindingWrappers.begin();
  IntToMysqlParameterMap::iterator stop = m_BindingWrappers.end();
  while (start != stop)
  {
    (*start).second->ClearBuffer();
    start++;
  }
}

