#include "../include/FirebirdResultSet.h"
#include "../include/FirebirdResultSetMetaData.h"
#include "../include/FirebirdDatabaseLayer.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayerException.h"

FirebirdResultSet::FirebirdResultSet(FirebirdInterface* pInterface)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pDatabase = NULL;
  m_pTransaction = NULL;
  m_pStatement = NULL;
  m_pFields = NULL;
  m_bManageStatement = false;
  m_bManageTransaction = false;
}

//FirebirdResultSet::FirebirdResultSet(const IBPP::Statement& statement)
FirebirdResultSet::FirebirdResultSet(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction, isc_stmt_handle pStatement, XSQLDA* pFields, bool bManageStmt /*= false*/, bool bManageTrans /*= false*/)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pDatabase = pDatabase;
  m_pTransaction = pTransaction;
  m_pStatement = pStatement;
  m_pFields = pFields;
  m_bManageStatement = bManageStmt;
  m_bManageTransaction = bManageTrans;

  AllocateFieldSpace();
  PopulateFieldLookupMap();
}

FirebirdResultSet::~FirebirdResultSet()
{
  Close();
}

bool FirebirdResultSet::Next()
{
  ResetErrorCodes();
  
  int nReturn = m_pInterface->GetIscDsqlFetch()(m_Status, &m_pStatement, 1, m_pFields);
  if (nReturn == 0) // No errors and retrieval successful
  {
    return true;
  }
  else if (nReturn == 100L) // No errors and no more rows
  {
    return false;
  }
  else  // Errors!!!
  {
    wxLogError(_("Error retrieving Next record\n"));
    InterpretErrorCodes();
    ThrowDatabaseException();
    return false;
  }
}

void FirebirdResultSet::Close()
{
  CloseMetaData();

  if (m_bManageTransaction && m_pTransaction)
  {
    int nReturn = m_pInterface->GetIscCommitTransaction()(m_Status, &m_pTransaction);
    // We're done with the transaction, so set it to NULL so that we know that a new transaction must be started if we run any queries
    m_pTransaction = NULL;
    if (nReturn != 0)
    {
      InterpretErrorCodes();
      ThrowDatabaseException();
    }
  }
  
  // Delete the statement if we have ownership of it
  if (m_bManageStatement && m_pStatement)
  {
    int nReturn = m_pInterface->GetIscDsqlFreeStatement()(m_Status, &m_pStatement, DSQL_drop);
    m_pStatement = NULL;
    if (nReturn != 0)
    {
      InterpretErrorCodes();
      ThrowDatabaseException();
    }
  }
  
  // Free the output fields structure
  if (m_pFields)
  {
    FreeFieldSpace();
    free(m_pFields);
    m_pFields = NULL;
  }
}


// get field
int FirebirdResultSet::GetResultInt(int nField)
{
  ResetErrorCodes();
  
  // Don't use nField-1 here since GetResultString will take care of that
  return GetResultLong(nField);
}

wxString FirebirdResultSet::GetResultString(int nField)
{
  ResetErrorCodes();
  
  wxString strReturn = wxEmptyString;
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  if (IsNull(pVar))
  {
    // The column is NULL
    strReturn = wxEmptyString;
  }
  else
  {
    short nType = pVar->sqltype & ~1;
    if (nType == SQL_TEXT)
    {
      strReturn = ConvertFromUnicodeStream(pVar->sqldata);
    }
    else if (nType == SQL_VARYING)
    {
      PARAMVARY* pVary = (PARAMVARY*) pVar->sqldata;
      pVary->vary_string[pVary->vary_length] = '\0';
      strReturn = ConvertFromUnicodeStream((const char*)pVary->vary_string);
    }
    else
    {
      // Incompatible field type
      // Set error codes and throw an exception here
      strReturn = _("");

      SetErrorMessage(_("Invalid field type"));
      SetErrorCode(DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE);

      ThrowDatabaseException();
    }
  }

  return strReturn;
}

long FirebirdResultSet::GetResultLong(int nField)
{
  ResetErrorCodes();
  
  long nReturn = 0L;
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  if (IsNull(pVar))
  {
    // The column is NULL
    nReturn = 0;
  }
  else
  {
    short nType = pVar->sqltype & ~1;
    if (nType == SQL_SHORT)
    {
      nReturn = *((short*)pVar->sqldata);
    }
    else if (nType == SQL_LONG)
    {
      nReturn = *((long*)pVar->sqldata);
    }
    else if (nType == SQL_INT64)
    {
      nReturn = *((ISC_INT64*)pVar->sqldata);
    }
    else
    {
      // Incompatible field type
      // We should really set error codes and throw an exception here
      nReturn = 0;

      SetErrorMessage(_("Invalid field type"));
      SetErrorCode(DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE);

      ThrowDatabaseException();
    }

    // Apply the scale to the value
    if (nReturn != 0)
    {
      short nScale = pVar->sqlscale;
      if (nScale > 0) // Multiply by 10
      {
        int nMultiplier = nScale * 10;
        nReturn *= nMultiplier;
      }
      else if (nScale < 0)  // Divide by 10
      {
        int nMultiplier = abs(nScale) * 10;
        nReturn /= nMultiplier;
      }
    }
  }

  return nReturn;
}

bool FirebirdResultSet::GetResultBool(int nField)
{
  ResetErrorCodes();
  
  // Don't use nField-1 here since GetResultString will take care of that
  int nValue = GetResultLong(nField);
  return (nValue != 0);
}

wxDateTime FirebirdResultSet::GetResultDate(int nField)
{
  ResetErrorCodes();
  
  wxDateTime dateReturn = wxInvalidDateTime;
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  if (IsNull(pVar))
  {
    // The column is NULL
    dateReturn = wxInvalidDateTime;
  }
  else
  {
    short nType = pVar->sqltype & ~1;
    if (nType == SQL_TIMESTAMP)
    {
      struct tm timeInTm;
		  m_pInterface->GetIscDecodeTimestamp()((ISC_TIMESTAMP *)pVar->sqldata, &timeInTm);
      SetDateTimeFromTm(dateReturn, timeInTm);
    }
    else if (nType == SQL_TYPE_DATE)
    {
      struct tm timeInTm;
		  m_pInterface->GetIscDecodeSqlDate()((ISC_DATE *)pVar->sqldata, &timeInTm);
      SetDateTimeFromTm(dateReturn, timeInTm);
    }
    else if (nType == SQL_TYPE_TIME)
    {
      struct tm timeInTm;
		  m_pInterface->GetIscDecodeSqlTime()((ISC_TIME *)pVar->sqldata, &timeInTm);
      SetDateTimeFromTm(dateReturn, timeInTm);
    }
    else
    {
      // Incompatible field type
      // Set error codes and throw an exception here
      dateReturn = wxInvalidDateTime;

      SetErrorMessage(_("Invalid field type"));
      SetErrorCode(DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE);

      ThrowDatabaseException();
    }
  }

  return dateReturn;
}

void FirebirdResultSet::SetDateTimeFromTm(wxDateTime& dateReturn, struct tm& timeInTm)
{
  dateReturn.Set(timeInTm.tm_mday, wxDateTime::Month(timeInTm.tm_mon), timeInTm.tm_year + 1900, timeInTm.tm_hour, timeInTm.tm_min, timeInTm.tm_sec);
}

double FirebirdResultSet::GetResultDouble(int nField)
{
  double dblReturn = 0.00;
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  if (IsNull(pVar))
  {
    // The column is NULL
    dblReturn = 0.00;
  }
  else
  {
    short nType = pVar->sqltype & ~1;
    if (nType == SQL_FLOAT)
    {
      dblReturn = *(float *) (pVar->sqldata);
    }
    else if (nType == SQL_DOUBLE)
    {
		  dblReturn = *(double *) (pVar->sqldata);
    }
    else if (nType == SQL_LONG)
    {
    	dblReturn = *(long *) (pVar->sqldata);
    	for(int i = 0; i < -pVar->sqlscale; dblReturn /= 10, i++);
    }
    else if (nType == SQL_INT64)
    {
    	dblReturn = *(ISC_INT64 *) (pVar->sqldata);
    	for(int i = 0; i < -pVar->sqlscale; dblReturn /= 10, i++);
    }
    else if (nType == SQL_SHORT)
    {
    	dblReturn = *(short *) (pVar->sqldata);
    	for(int i = 0; i < -pVar->sqlscale; dblReturn /= 10, i++);
    }
    else
    {
      // Incompatible field type
      // Set error codes and throw an exception here
      dblReturn = 0.00;
        
      SetErrorMessage(_("Invalid field type"));
      SetErrorCode(DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE);

      ThrowDatabaseException();
    }
  }

  return dblReturn;
}

void* FirebirdResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  ResetErrorCodes();
  
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  if (IsNull(pVar))
  {
    // The column is NULL
    wxMemoryBuffer tempBuffer(0);
    tempBuffer.SetBufSize(0);
    tempBuffer.SetDataLen(0); 
    Buffer = tempBuffer;
    return NULL;
  }
  else
  {
    short nType = pVar->sqltype & ~1;
    if (nType == SQL_BLOB)
    {
      ISC_QUAD blobId = *(ISC_QUAD *) pVar->sqldata;
      isc_blob_handle pBlob = NULL;
      char szSegment[128];
      unsigned short nSegmentLength;
      m_pInterface->GetIscOpenBlob2()(m_Status, &m_pDatabase, &m_pTransaction, &pBlob, &blobId, 0, NULL);

      ISC_STATUS blobStatus = m_pInterface->GetIscGetSegment()(m_Status, &pBlob, &nSegmentLength, sizeof(szSegment), szSegment);
      wxMemoryBuffer tempBuffer(nSegmentLength);
      size_t bufferSize = 0;
      while (blobStatus == 0 || m_Status[1] == isc_segment)
      {
        tempBuffer.AppendData(szSegment, nSegmentLength);
        bufferSize += nSegmentLength;
        blobStatus = m_pInterface->GetIscGetSegment()(m_Status, &pBlob, &nSegmentLength, sizeof(szSegment), szSegment);
      }
      m_pInterface->GetIscCloseBlob()(m_Status, &pBlob);

      // Some memory buffer juggling to make sure there's no extra space allocated
      tempBuffer.SetDataLen(bufferSize);
      tempBuffer.SetBufSize(bufferSize);
      wxMemoryBuffer tempBufferExactSize(bufferSize);
      void* pBuffer = tempBufferExactSize.GetWriteBuf(bufferSize);
      memcpy(pBuffer, tempBuffer.GetData(), bufferSize);
      tempBufferExactSize.UngetWriteBuf(bufferSize);
      tempBufferExactSize.SetDataLen(bufferSize);
      tempBufferExactSize.SetBufSize(bufferSize);
      Buffer = tempBufferExactSize;
    }
    else
    {
      // Incompatible field type
      // Set error codes and throw an exception here
      wxMemoryBuffer tempBuffer(0);
      tempBuffer.SetBufSize(0);
      tempBuffer.SetDataLen(0); 
      Buffer = tempBuffer;

      SetErrorMessage(_("Invalid field type"));
      SetErrorCode(DATABASE_LAYER_INCOMPATIBLE_FIELD_TYPE);

      ThrowDatabaseException();
    }
  }

  return Buffer.GetData();
}

bool FirebirdResultSet::IsFieldNull(int nField)
{
  XSQLVAR* pVar = &(m_pFields->sqlvar[nField-1]);
  return IsNull(pVar);
}

bool FirebirdResultSet::IsNull(XSQLVAR* pVar)
{
  return ((pVar->sqltype & 1) && (*pVar->sqlind < 0));
}

/*
FirebirdResultSet& FirebirdResultSet::operator=(const IBPP::Statement& statement)
{
  m_ResultSet = statement;
  return *this;
}
*/
void FirebirdResultSet::AllocateFieldSpace()
{
  if (m_pFields == NULL)
    return;

  for (int i = 0; i < m_pFields->sqld; i++)
  {
    XSQLVAR* pVar = &(m_pFields->sqlvar[i]);
    switch (pVar->sqltype & ~1)
    {
      case SQL_ARRAY:
      case SQL_BLOB:
        pVar->sqldata = (char*)new ISC_QUAD;
        memset(pVar->sqldata, 0, sizeof(ISC_QUAD));
        break;
      case SQL_TIMESTAMP:
        pVar->sqldata = (char*)new ISC_TIMESTAMP;
        memset(pVar->sqldata, 0, sizeof(ISC_TIMESTAMP));
        break;
      case SQL_TYPE_TIME:
        pVar->sqldata = (char*)new ISC_TIME;
        memset(pVar->sqldata, 0, sizeof(ISC_TIME));
        break;
      case SQL_TYPE_DATE:
        pVar->sqldata = (char*)new ISC_DATE;
        memset(pVar->sqldata, 0, sizeof(ISC_DATE));
        break;
      case SQL_TEXT:
        pVar->sqldata = new char[pVar->sqllen+1];
        memset(pVar->sqldata, '\0', pVar->sqllen);
        pVar->sqldata[pVar->sqllen] = '\0';
        break;
      case SQL_VARYING:
        pVar->sqldata = new char[pVar->sqllen+3];
        memset(pVar->sqldata, 0, 2);
        memset(pVar->sqldata+2, '\0', pVar->sqllen);
        pVar->sqldata[pVar->sqllen+2] = '\0';
        break;
      case SQL_SHORT:
        pVar->sqldata = (char*)new short(0);
        break;
      case SQL_LONG:
        pVar->sqldata = (char*)new long(0);
        break;
      case SQL_INT64:
        pVar->sqldata = (char*)new ISC_INT64(0);
        break;
      case SQL_FLOAT:
        pVar->sqldata = (char*)new float(0.0);
        break;
      case SQL_DOUBLE:
        pVar->sqldata = (char*)new double(0.0);
        break;
      default : 
        break;
    }
    if (pVar->sqltype & 1)
      pVar->sqlind = new short(-1);	// 0 indicator
  }
}

void FirebirdResultSet::FreeFieldSpace()
{
	if (m_pFields == NULL)
    return;

  for (int i = 0; i < m_pFields->sqln; i++)
  {
    XSQLVAR* pVar = &(m_pFields->sqlvar[i]);
    if (pVar->sqldata != 0)
    {
      switch (pVar->sqltype & ~1)
      {
        case SQL_ARRAY:
        case SQL_BLOB:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_TIMESTAMP:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_TYPE_TIME:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_TYPE_DATE:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_TEXT:
        case SQL_VARYING:
          wxDELETEA(pVar->sqldata);
          break;
        case SQL_SHORT:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_LONG:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_INT64:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_FLOAT:
          wxDELETE(pVar->sqldata);
          break;
        case SQL_DOUBLE:
          wxDELETE(pVar->sqldata);
          break;
        default:
          break;
      }
    }
    if ((pVar->sqltype & 1) && (pVar->sqlind != 0))
      delete pVar->sqlind;
  }

  //delete [] (char*)m_pFields;
  //m_pFields = NULL;
  wxDELETEA(m_pFields);
}

void FirebirdResultSet::PopulateFieldLookupMap()
{
  m_FieldLookupMap.clear();

  // Iterate through all the XSQLVAR structures mapping their aliasname fields to their positions in the XSQLDA->sqlvar array
  XSQLVAR* pVar = m_pFields->sqlvar;
  for (int i=0; i<m_pFields->sqld; i++, pVar++)
  {
    wxString strField = ConvertFromUnicodeStream(pVar->aliasname);
    m_FieldLookupMap[strField] = i;
  }
}

int FirebirdResultSet::LookupField(const wxString& strField)
{
  wxString strUpperCaseCopy = strField;
  strUpperCaseCopy.MakeUpper();
  StringToIntMap::iterator SearchIterator = m_FieldLookupMap.find(strUpperCaseCopy);
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
    return ((*SearchIterator).second+1); // Add +1 to make the result set 1-based rather than 0-based
  }
}

void FirebirdResultSet::InterpretErrorCodes()
{
  wxLogError(_("FirebirdResultSet::InterpretErrorCodes()\n"));

  long nSqlCode = m_pInterface->GetIscSqlcode()(m_Status);
  SetErrorCode(FirebirdDatabaseLayer::TranslateErrorCode(nSqlCode));
  SetErrorMessage(FirebirdDatabaseLayer::TranslateErrorCodeToString(m_pInterface, nSqlCode, m_Status));
}

ResultSetMetaData* FirebirdResultSet::GetMetaData()
{
  ResultSetMetaData* pMetaData = new FirebirdResultSetMetaData(m_pFields);
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}


