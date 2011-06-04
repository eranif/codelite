#include "../include/FirebirdParameter.h"
#include "../include/FirebirdDatabaseLayer.h"
#include "../include/DatabaseLayerException.h"

// ctor
FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_nNullFlag = -1;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
  m_nParameterType = FirebirdParameter::PARAM_NULL;
  m_pDatabase = NULL;
  m_pTransaction = NULL;
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, const wxString& strValue, const wxCSConv* conv)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_strValue = strValue;

  SetEncoding(conv);

  // Set to SQL_TEXT manually
  m_pParameter->sqltype = SQL_TEXT;
  wxCharBuffer valueBuffer = ConvertToUnicodeStream(m_strValue);
  size_t length = GetEncodedStreamLength(m_strValue);
  wxStrncpy((wxChar*)m_pParameter->sqldata, (wxChar*)(const char*)valueBuffer, length);
  //(char*)(m_pParameter->sqldata) = valueBuffer;
  m_pParameter->sqllen = length;

  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, int nValue)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_nParameterType = FirebirdParameter::PARAM_INT;
  m_nValue = nValue;

  m_pParameter->sqldata = (char*)&m_nValue;

  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, double dblValue)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_nParameterType = FirebirdParameter::PARAM_DOUBLE;
  int nType = (m_pParameter->sqltype & ~1);
  if (nType == SQL_FLOAT)
  {
    m_fValue = dblValue;
    m_pParameter->sqldata = (char*)&m_fValue;
  }
  else if (nType == SQL_DOUBLE)
  {
    m_dblValue = dblValue;
    m_pParameter->sqldata = (char*)&m_dblValue;
  }
  else if(nType == SQL_LONG || nType == SQL_INT64 || nType == SQL_SHORT)
  {
    for(int i = 0; i < -pVar->sqlscale; dblValue *= 10, i++);
    m_nValue = dblValue;
    m_pParameter->sqldata = (char*)&m_nValue;
  }
  else
  {
    // Error?
    wxLogError(_("Parameter type is not compatible with parameter of type double\n"));
  }
  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, bool bValue)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_nParameterType = FirebirdParameter::PARAM_BOOL;
  m_bValue = bValue;
  m_nValue = (m_bValue) ? 1 : 0;

  m_pParameter->sqldata = (char*)&m_nValue;

  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, const wxDateTime& dateValue)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_nParameterType = FirebirdParameter::PARAM_DATETIME;

  struct tm dateAsTm;
  wxDateTime::Tm tm = dateValue.GetTm();
  dateAsTm.tm_sec = tm.sec;
  dateAsTm.tm_min = tm.min;
  dateAsTm.tm_hour = tm.hour;
  dateAsTm.tm_mday = tm.mday;
  dateAsTm.tm_mon = tm.mon;
  dateAsTm.tm_year = tm.year - 1900;
  m_pInterface->GetIscEncodeTimestamp()(&dateAsTm, &m_Date);

  m_nBufferLength = sizeof(ISC_TIMESTAMP);

  m_pParameter->sqldata = (char*)&m_Date;

  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, isc_db_handle pDatabase, isc_tr_handle pTransaction, const void* pData, long nDataLength)
{
  m_pInterface = pInterface;
  m_pParameter = pVar;
  m_pDatabase = pDatabase;
  m_pTransaction = pTransaction;

  // Just copy the data into the memory buffer for now.  We'll move the data over to the blob in the call to ResetBlob
  void* pBuffer = m_BufferValue.GetWriteBuf(nDataLength);
  memcpy(pBuffer, pData, nDataLength);
  m_nBufferLength = nDataLength;
}

void FirebirdParameter::ResetBlob()
{
  // If the databaes and transaction handles aren't valid then don't try to do anything
  if ((m_pDatabase == NULL) || (m_pTransaction == NULL))
    return;

  //m_BlobId = NULL;
  m_pBlob = NULL;
  ISC_STATUS_ARRAY    status;              /* status vector */
  void* pData = m_BufferValue.GetData();
  int nDataLength = m_nBufferLength;//m_BufferValue.GetDataLen();

  memset(&m_BlobId, 0, sizeof(m_BlobId));
  int nReturn = m_pInterface->GetIscCreateBlob2()(status, &m_pDatabase, &m_pTransaction, &m_pBlob, &m_BlobId, 0, NULL);
  if (nReturn != 0)
  {
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    long nSqlCode = m_pInterface->GetIscSqlcode()(status);
    DatabaseLayerException error(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode),
        FirebirdDatabaseLayer::TranslateErrorCodeToString(m_pInterface, nSqlCode, status));

    throw error;
#endif

    //isc_print_status(status);
    return;
  }

  int dataFetched = 0;
  char* dataPtr = (char*)pData;
  while (dataFetched < nDataLength)
  {
    unsigned short segLen = (nDataLength - dataFetched) < 0xFFFF ? (nDataLength - dataFetched) : 0xFFFF ;
    nReturn = m_pInterface->GetIscPutSegment()(status, &m_pBlob, segLen, dataPtr);
    if (nReturn != 0)
    {
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
      long nSqlCode = m_pInterface->GetIscSqlcode()(status);
      DatabaseLayerException error(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode),
        FirebirdDatabaseLayer::TranslateErrorCodeToString(m_pInterface, nSqlCode, status));

      throw error;
#endif

      //isc_print_status(status);
      return;
    }

    dataFetched += segLen;
    dataPtr += segLen;
  }

  nReturn = m_pInterface->GetIscCloseBlob()(status, &m_pBlob);
  if (nReturn != 0)
  {
#ifndef DONT_USE_DATABASE_LAYER_EXCEPTIONS
    long nSqlCode = m_pInterface->GetIscSqlcode()(status);
    DatabaseLayerException error(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode),
        FirebirdDatabaseLayer::TranslateErrorCodeToString(m_pInterface, nSqlCode, status));

    throw error;
#endif

    //isc_print_status(status);
    return;
  }

  m_pParameter->sqldata = (char*)&m_BlobId;

  m_nNullFlag = 0;
  m_pParameter->sqlind = &m_nNullFlag; // NULL indicator
}

FirebirdParameter::~FirebirdParameter()
{
}

