#include "../include/MysqlParameter.h"
#include "../include/DatabaseLayer.h"

// ctor
MysqlParameter::MysqlParameter()
{
  m_nParameterType = MysqlParameter::PARAM_NULL;
}

MysqlParameter::MysqlParameter(const wxString& strValue)
{
  m_nParameterType = MysqlParameter::PARAM_STRING;
  m_strValue = strValue;
  m_CharBufferValue = ConvertToUnicodeStream(m_strValue);
  if (_("") == strValue)
  {
    m_nBufferLength = 0;
  }
  else
  {
    m_nBufferLength = GetEncodedStreamLength(m_strValue);
  }
}

MysqlParameter::MysqlParameter(int nValue)
{
  m_nParameterType = MysqlParameter::PARAM_INT;
  m_nValue = nValue;
  //m_strValue = wxString::Format(_("%d"), nValue);
}

MysqlParameter::MysqlParameter(double dblValue)
{
  m_nParameterType = MysqlParameter::PARAM_DOUBLE;
  m_dblValue = dblValue;
  //m_strValue = wxString::Format(_("%f"), dblValue);
}

MysqlParameter::MysqlParameter(bool bValue)
{
  m_nParameterType = MysqlParameter::PARAM_BOOL;
  m_bValue = bValue;
}

MysqlParameter::MysqlParameter(const wxDateTime& dateValue)
{
  m_nParameterType = MysqlParameter::PARAM_DATETIME;

  m_pDate = new MYSQL_TIME();

  memset(m_pDate, 0, sizeof(MYSQL_TIME));

  m_pDate->year = dateValue.GetYear();
  m_pDate->month = dateValue.GetMonth()+1;
  m_pDate->day = dateValue.GetDay();
  m_pDate->hour = dateValue.GetHour();
  m_pDate->minute = dateValue.GetMinute();
  m_pDate->second = dateValue.GetSecond();
  m_pDate->neg = 0;

  m_nBufferLength = sizeof(MYSQL_TIME);
}

MysqlParameter::MysqlParameter(const void* pData, long nDataLength)
{
  m_nParameterType = MysqlParameter::PARAM_BLOB;
  void* pBuffer = m_BufferValue.GetWriteBuf(nDataLength);
  memcpy(pBuffer, pData, nDataLength);
  m_nBufferLength = nDataLength;
}

MysqlParameter::~MysqlParameter()
{
  if ((m_nParameterType == MysqlParameter::PARAM_DATETIME) && (m_pDate != NULL))
  {
    delete m_pDate;
    m_pDate = NULL;
  }
}

long unsigned int MysqlParameter::GetDataLength()
{
  return m_nBufferLength;
}

long unsigned int* MysqlParameter::GetDataLengthPtr()
{
  return &m_nBufferLength;
}

const void* MysqlParameter::GetDataPtr()
{
  const void *pReturn = NULL;
  
  switch (m_nParameterType)
  {
    case MysqlParameter::PARAM_STRING:
      pReturn = m_CharBufferValue;
      break;
    case MysqlParameter::PARAM_INT:
      pReturn = &m_nValue;
      break;
    case MysqlParameter::PARAM_DOUBLE:
      pReturn = &m_dblValue;
      break;
    case MysqlParameter::PARAM_DATETIME:
      pReturn = m_pDate;
      break;
    case MysqlParameter::PARAM_BOOL:
      pReturn = &m_bValue;
      break;
    case MysqlParameter::PARAM_BLOB:
      pReturn = m_BufferValue.GetData();
      break;
    case MysqlParameter::PARAM_NULL:
      pReturn = NULL;
      break;
    default:
      pReturn = NULL;
      break;
  };
  return pReturn;
}

int MysqlParameter::GetParameterType()
{
  return m_nParameterType;
}

enum_field_types MysqlParameter::GetBufferType()
{
  enum_field_types returnType = MYSQL_TYPE_NULL;

  switch (m_nParameterType)
  {
    case MysqlParameter::PARAM_STRING:
      returnType = MYSQL_TYPE_VAR_STRING;
      break;
    case MysqlParameter::PARAM_INT:
      returnType = MYSQL_TYPE_LONG;
      break;
    case MysqlParameter::PARAM_DOUBLE:
      returnType = MYSQL_TYPE_DOUBLE;
      break;
    case MysqlParameter::PARAM_DATETIME:
      returnType = MYSQL_TYPE_TIMESTAMP;
      break;
    case MysqlParameter::PARAM_BOOL:
      returnType = MYSQL_TYPE_TINY;
      break;
    case MysqlParameter::PARAM_BLOB:
      returnType = MYSQL_TYPE_LONG_BLOB;
      break;
    case MysqlParameter::PARAM_NULL:
      returnType = MYSQL_TYPE_NULL;
      break;
    default:
      returnType = MYSQL_TYPE_NULL;
      break;
  };

  return returnType;
}

