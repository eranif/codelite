#include "../include/PostgresParameter.h"
#include "../include/DatabaseLayer.h"

// ctor
PostgresParameter::PostgresParameter()
{
  m_nParameterType = PostgresParameter::PARAM_NULL;
}

PostgresParameter::PostgresParameter(const wxString& strValue)
{
  m_nParameterType = PostgresParameter::PARAM_STRING;
  m_strValue = strValue;
  m_nBufferLength = m_strValue.Length();
}

PostgresParameter::PostgresParameter(int nValue)
{
  m_nParameterType = PostgresParameter::PARAM_INT;
  //m_nValue = nValue;
  m_strValue = wxString::Format(_("%d"), nValue);
}

PostgresParameter::PostgresParameter(double dblValue)
{
  m_nParameterType = PostgresParameter::PARAM_DOUBLE;
  //m_dblValue = dblValue;
  m_strValue = wxString::Format(_("%f"), dblValue);
}

PostgresParameter::PostgresParameter(bool bValue)
{
  m_nParameterType = PostgresParameter::PARAM_BOOL;
  m_bValue = bValue;
}

PostgresParameter::PostgresParameter(const wxDateTime& dateValue)
{
  m_nParameterType = PostgresParameter::PARAM_DATETIME;
  m_strDateValue = dateValue.Format(_("%Y-%m-%d %H:%M:%S"));
  m_nBufferLength = m_strDateValue.Length();
}

PostgresParameter::PostgresParameter(const void* pData, long nDataLength)
{
  m_nParameterType = PostgresParameter::PARAM_BLOB;
  void* pBuffer = m_BufferValue.GetWriteBuf(nDataLength);
  memcpy(pBuffer, pData, nDataLength);
  m_nBufferLength = nDataLength;
}


long PostgresParameter::GetDataLength()
{
  return m_nBufferLength;
}

long* PostgresParameter::GetDataLengthPointer()
{
  return &m_nBufferLength;
}

const void* PostgresParameter::GetDataPtr()
{
  const void *pReturn = NULL;
  
  switch (m_nParameterType)
  {
    case PostgresParameter::PARAM_STRING:
      m_CharBufferValue = ConvertToUnicodeStream(m_strValue);
      pReturn = m_CharBufferValue;
      break;
    case PostgresParameter::PARAM_INT:
      //pReturn = &m_nValue;
      m_CharBufferValue = ConvertToUnicodeStream(m_strValue);
      pReturn = m_CharBufferValue;
      break;
    case PostgresParameter::PARAM_DOUBLE:
      //pReturn = &m_dblValue;
      m_CharBufferValue = ConvertToUnicodeStream(m_strValue);
      pReturn = m_CharBufferValue;
      break;
    case PostgresParameter::PARAM_DATETIME:
      m_CharBufferValue = ConvertToUnicodeStream(m_strDateValue);
      pReturn = m_CharBufferValue;
      break;
    case PostgresParameter::PARAM_BOOL:
      pReturn = &m_bValue;
      break;
    case PostgresParameter::PARAM_BLOB:
      pReturn = m_BufferValue.GetData();
      break;
    case PostgresParameter::PARAM_NULL:
      pReturn = NULL;
      break;
    default:
      pReturn = NULL;
      break;
  };
  return pReturn;
}

int PostgresParameter::GetParameterType()
{
  return m_nParameterType;
}

bool PostgresParameter::IsBinary()
{
  return (PostgresParameter::PARAM_BLOB == m_nParameterType);
}

