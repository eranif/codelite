#include "../include/OdbcParameter.h"
#include "../include/DatabaseLayer.h"

#include <sqlext.h>

// ctor
OdbcParameter::OdbcParameter()
{
  m_nParameterType = OdbcParameter::PARAM_NULL;
  m_nBufferLength = SQL_NULL_DATA;
}

OdbcParameter::OdbcParameter(const wxString& strValue)
{
  m_nParameterType = OdbcParameter::PARAM_STRING;
  m_strValue = strValue;
  m_nBufferLength = GetEncodedStreamLength(m_strValue);
}

OdbcParameter::OdbcParameter(int nValue)
{
  m_nParameterType = OdbcParameter::PARAM_INT;
  m_nValue = nValue;
  m_nBufferLength = 0;
}

OdbcParameter::OdbcParameter(double dblValue)
{
  m_nParameterType = OdbcParameter::PARAM_DOUBLE;
  m_dblValue = dblValue;
  m_nBufferLength = 0;
}

OdbcParameter::OdbcParameter(bool bValue)
{
  m_nParameterType = OdbcParameter::PARAM_BOOL;
  m_bValue = bValue;
  m_nBufferLength = 0;
}

OdbcParameter::OdbcParameter(const wxDateTime& dateValue)
{
  m_nParameterType = OdbcParameter::PARAM_DATETIME;
    	      
  m_DateValue.year = dateValue.GetYear();
  m_DateValue.month = dateValue.GetMonth()+1;
	m_DateValue.day = dateValue.GetDay();
    	    
  m_DateValue.hour = dateValue.GetHour();
  m_DateValue.minute = dateValue.GetMinute();
  m_DateValue.second = dateValue.GetSecond();
  m_DateValue.fraction = dateValue.GetMillisecond();

  m_nBufferLength = 0;
}

OdbcParameter::OdbcParameter(const void* pData, long nDataLength)
{
  m_nParameterType = OdbcParameter::PARAM_BLOB;
  void* pBuffer = m_BufferValue.GetWriteBuf(nDataLength);
  memcpy(pBuffer, pData, nDataLength);
  m_nBufferLength = nDataLength;
}


long OdbcParameter::GetDataLength()
{
  if (m_nParameterType == OdbcParameter::PARAM_NULL)
  {
    return 0;
  }
  else
  {
    return m_nBufferLength;
  }
}

long* OdbcParameter::GetDataLengthPointer()
{
  if (m_nParameterType == OdbcParameter::PARAM_NULL)
  {
    return &m_nBufferLength;
  }
  else
  {
    return NULL;
  }
}

void* OdbcParameter::GetDataPtr()
{
  const void *pReturn = NULL;
  
  switch (m_nParameterType)
  {
    case OdbcParameter::PARAM_STRING:
      m_CharBufferValue = ConvertToUnicodeStream(m_strValue);
      pReturn = m_CharBufferValue;
      //wxPrintf(_("Parameter: '%s'\n"), m_strValue.c_str());
      //pReturn = m_strValue.c_str();
      break;
    case OdbcParameter::PARAM_INT:
      pReturn = &m_nValue;
      break;
    case OdbcParameter::PARAM_DOUBLE:
      pReturn = &m_dblValue;
      break;
    case OdbcParameter::PARAM_DATETIME:
      pReturn = &m_DateValue;
      break;
    case OdbcParameter::PARAM_BOOL:
      pReturn = &m_bValue;
      break;
    case OdbcParameter::PARAM_BLOB:
      pReturn = m_BufferValue.GetData();
      break;
    case OdbcParameter::PARAM_NULL:
      pReturn = NULL;
      break;
    default:
      pReturn = NULL;
      break;
  };
  return (void*)pReturn;
}

SQLSMALLINT OdbcParameter::GetValueType()
{
  SQLSMALLINT nReturn = SQL_C_LONG;
  
  switch (m_nParameterType)
  {
    case OdbcParameter::PARAM_STRING:
      nReturn = SQL_C_CHAR;
      break;
    case OdbcParameter::PARAM_INT:
      nReturn = SQL_C_LONG;
      break;
    case OdbcParameter::PARAM_DOUBLE:
      nReturn = SQL_C_DOUBLE;
      break;
    case OdbcParameter::PARAM_DATETIME:
      nReturn = SQL_C_TYPE_TIMESTAMP;
      break;
    case OdbcParameter::PARAM_BOOL:
      nReturn = SQL_C_LONG;
      break;
    case OdbcParameter::PARAM_BLOB:
      nReturn = SQL_C_BINARY;
      break;
    case OdbcParameter::PARAM_NULL:
      nReturn = SQL_C_CHAR;
      break;
    default:
      nReturn = SQL_C_NUMERIC;
      break;
  };
  return nReturn;
}

SQLSMALLINT OdbcParameter::GetParameterType()
{
  SQLSMALLINT nReturn = SQL_INTEGER;
  
  switch (m_nParameterType)
  {
    case OdbcParameter::PARAM_STRING:
      nReturn = SQL_VARCHAR;
      break;
    case OdbcParameter::PARAM_INT:
      nReturn = SQL_INTEGER;
      break;
    case OdbcParameter::PARAM_DOUBLE:
      nReturn = SQL_DOUBLE;
      break;
    case OdbcParameter::PARAM_DATETIME:
      //nReturn = SQL_TYPE_TIMESTAMP;
      nReturn = SQL_TIMESTAMP;
      break;
    case OdbcParameter::PARAM_BOOL:
      nReturn = SQL_INTEGER;
      break;
    case OdbcParameter::PARAM_BLOB:
      nReturn = SQL_BINARY;
      //nReturn = SQL_LONGVARBINARY;
      break;
    case OdbcParameter::PARAM_NULL:
      nReturn = SQL_NULL_DATA;
      break;
    default:
      nReturn = SQL_INTEGER;
      break;
  };
  return nReturn;

}

bool OdbcParameter::IsBinary()
{
  return (OdbcParameter::PARAM_BLOB == m_nParameterType);
}

SQLSMALLINT OdbcParameter::GetDecimalDigits()
{
  // either decimal_digits or 0 (date, bool, int)
  SQLSMALLINT nReturn = 0;
  
  switch (m_nParameterType)
  {
    case OdbcParameter::PARAM_STRING:
    case OdbcParameter::PARAM_BLOB:
    case OdbcParameter::PARAM_INT:
    case OdbcParameter::PARAM_DATETIME:
    case OdbcParameter::PARAM_BOOL:
    case OdbcParameter::PARAM_NULL:
      nReturn = 0;
      break;
    case OdbcParameter::PARAM_DOUBLE:
      nReturn = 11;
      break;
    default:
      nReturn = 0;
      break;
  };
  return nReturn;
}

SQLUINTEGER OdbcParameter::GetColumnSize()
{
  SQLUINTEGER nReturn = 0;

  switch (m_nParameterType)
  {
    case OdbcParameter::PARAM_STRING:
    case OdbcParameter::PARAM_BLOB:
      nReturn = m_nBufferLength;
      break;
    case OdbcParameter::PARAM_INT:
    case OdbcParameter::PARAM_DOUBLE:
    case OdbcParameter::PARAM_BOOL:
    case OdbcParameter::PARAM_NULL:
      nReturn = 0;
      break;
    case OdbcParameter::PARAM_DATETIME:
      nReturn = 19;
      break;
    default:
      nReturn = 0;
      break;
  };
  return nReturn;
}

SQLINTEGER* OdbcParameter::GetParameterLengthPtr()
{
  return &m_nBufferLength;
}
