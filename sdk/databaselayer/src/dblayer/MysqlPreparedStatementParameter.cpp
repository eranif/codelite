#include "../include/MysqlPreparedStatementParameter.h"
#include "../include/MysqlDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"

// ctor
MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  // Default to NULL
  SetNull();
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const wxString& strValue)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetString(strValue);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, int nValue)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetInt(nValue);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, double dblValue)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetDouble(dblValue);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const void* pData, long nDataLength)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetBlob(pData, nDataLength);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const wxDateTime& dateValue)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetDate(dateValue);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, bool bValue)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  SetBool(bValue);
}

MysqlPreparedStatementParameter::MysqlPreparedStatementParameter(MYSQL_BIND* pBind, MYSQL_FIELD* pField)
 : DatabaseErrorReporter()
{
  m_pBind = pBind;
  m_pBind->is_null = &m_bIsNull;

  // Set the binding properties
  m_pBind->buffer_type = pField->type;
  m_pBind->buffer_length = pField->length + 1;
  //int nType = m_pBind->buffer_type;
  //if (nType == MYSQL_TYPE_STRING || nType == MYSQL_TYPE_VAR_STRING || nType == MYSQL_TYPE_BLOB
  //  || nType == MYSQL_TYPE_TINY_BLOB || nType == MYSQL_TYPE_MEDIUM_BLOB || nType == MYSQL_TYPE_LONG_BLOB)
  //{
    //wxLogDebug(_("Allocating %ld bytes in the MYSQL_BIND buffer\n"), pField->length);
    void* pBuffer = m_Data.bufferValue.GetWriteBuf(m_pBind->buffer_length);
    if (pBuffer == 0)
    {
      SetErrorCode(MysqlDatabaseLayer::TranslateErrorCode(0));
      SetErrorMessage(_("Error allocating buffer"));
      ThrowDatabaseException();
    }
    m_pBind->buffer = pBuffer;
    ClearBuffer();
  //  }
  //  else
  //  {
  //  pCurrentBinding->buffer = malloc(1);
  //  }

  int nType = m_pBind->buffer_type;
  if (nType == MYSQL_TYPE_BLOB || nType == MYSQL_TYPE_TINY_BLOB || nType == MYSQL_TYPE_MEDIUM_BLOB 
    || nType == MYSQL_TYPE_LONG_BLOB)
    m_pBind->length = &m_Data.nBufferLength;
}

// dtor
MysqlPreparedStatementParameter::~MysqlPreparedStatementParameter()
{
}

void MysqlPreparedStatementParameter::ClearBuffer()
{
  if (m_pBind && m_pBind->buffer && (m_pBind->buffer_length > 0))
  {
    memset(m_pBind->buffer, 0, m_pBind->buffer_length);
  }
}

void MysqlPreparedStatementParameter::SetInt(int nValue)
{
  m_Data.nValue = nValue;
  m_pBind->buffer_type = MYSQL_TYPE_LONG;
  m_pBind->buffer = (void*)&m_Data.nValue;
}

void MysqlPreparedStatementParameter::SetDouble(double dblValue)
{
  m_Data.dblValue = dblValue;
  m_pBind->buffer_type = MYSQL_TYPE_DOUBLE;
  m_pBind->buffer = (void*)&m_Data.dblValue;
}

void MysqlPreparedStatementParameter::SetString(const wxString& strValue)
{
  memset(m_pBind, 0, sizeof(MYSQL_BIND));
  m_Data.strValue = strValue;
  m_pBind->buffer_type = MYSQL_TYPE_STRING;
  m_Data.charBufferValue = ConvertToUnicodeStream(m_Data.strValue);
  m_pBind->buffer = (void*)(const char*)m_Data.charBufferValue;
  m_Data.nBufferLength = GetEncodedStreamLength(m_Data.strValue);
  m_pBind->length = &(m_Data.nBufferLength);
  m_pBind->buffer_length = m_Data.nBufferLength;
}

void MysqlPreparedStatementParameter::SetNull()
{
  m_bIsNull = 1;
  m_pBind->is_null = &m_bIsNull;
}

void MysqlPreparedStatementParameter::SetBlob(const void* pData, long nDataLength)
{
  void* pBuffer = m_Data.bufferValue.GetWriteBuf(nDataLength);
  memcpy(pBuffer, pData, nDataLength);
  m_pBind->buffer_type = MYSQL_TYPE_BLOB;
  m_pBind->buffer = pBuffer;
  m_pBind->buffer_length = nDataLength;
}

void MysqlPreparedStatementParameter::SetDate(const wxDateTime& dateValue)
{
  m_Data.dateValue.year = dateValue.GetYear();
  m_Data.dateValue.month = dateValue.GetMonth();
  m_Data.dateValue.day = dateValue.GetDay();
  m_Data.dateValue.hour = dateValue.GetHour();
  m_Data.dateValue.minute = dateValue.GetMinute();
  m_Data.dateValue.second = dateValue.GetSecond();
    
  m_pBind->buffer_type = MYSQL_TYPE_DATETIME;
  m_pBind->buffer = (void*)&m_Data.dateValue;
}

void MysqlPreparedStatementParameter::SetBool(bool bValue)
{
  m_Data.bValue = bValue;
  m_pBind->buffer_type = MYSQL_TYPE_TINY;
  m_pBind->buffer = (void*)&m_Data.bValue;
}

