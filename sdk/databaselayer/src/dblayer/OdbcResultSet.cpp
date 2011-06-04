#include "../include/OdbcResultSet.h"
#include "../include/OdbcPreparedStatement.h"
#include "../include/OdbcDatabaseLayer.h"
#include "../include/OdbcResultSetMetaData.h"
#include "../include/DatabaseLayerException.h"
#include "../include/DatabaseErrorCodes.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(ValuesArray);

#define ODBC_FIELD_NAME_LEN 71

// ctor
OdbcResultSet::OdbcResultSet(OdbcInterface* pInterface)
 : DatabaseResultSet()
{
    m_pInterface = pInterface;
    m_pStatement = NULL;
    m_pOdbcStatement = NULL;
    m_bManageStatement = false;
}

OdbcResultSet::OdbcResultSet(OdbcInterface* pInterface, OdbcPreparedStatement* pStatement, bool bManageStatement, int nCol)
 : DatabaseResultSet()
{
    m_pInterface = pInterface;
    m_pStatement = pStatement;
    m_pOdbcStatement = m_pStatement->GetLastStatement();
    m_bManageStatement = bManageStatement;

    // Populate field lookup map
    SQLTCHAR field_name[ODBC_FIELD_NAME_LEN];
    SQLSMALLINT colnamelen;
   
    for (int i=0; i<nCol; i++)
    {
        UWORD col = i+1;
        long nReturn = m_pInterface->GetSQLColAttributes()(m_pOdbcStatement, col, SQL_COLUMN_NAME,
	                      field_name,
                          ODBC_FIELD_NAME_LEN, &colnamelen, 0);
        if ( nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO )
        {
            InterpretErrorCodes(nReturn, m_pOdbcStatement);
            ThrowDatabaseException();
            return;
        }
        
#if wxUSE_UNICODE
		wxString strField = ConvertFromUnicodeStream((const char*)(wxChar*)field_name);
#else
		wxString strField((wxChar*)field_name);
#endif
        m_FieldLookupMap[strField.Upper()] = i;
    }
}

// dtor
OdbcResultSet::~OdbcResultSet()
{
    Close();

    m_FieldLookupMap.clear();
    m_fieldValues.Clear();

    m_RetrievedValues.clear();
    m_NullValues.clear();

    m_BlobMap.clear();
}


void OdbcResultSet::Close()
{
    CloseMetaData();

    if (m_bManageStatement)
    {
        if (m_pStatement != NULL)
        {
            //m_pStatement->Close();
            wxDELETE(m_pStatement);
        }
    }
}


bool OdbcResultSet::Next()
{
    m_RetrievedValues.clear();
    m_NullValues.clear();

    m_BlobMap.clear();

    if (m_pOdbcStatement == NULL)
        m_pOdbcStatement = m_pStatement->GetLastStatement();

    long nReturn = m_pInterface->GetSQLFetch()( m_pOdbcStatement );
     
    if ( nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO )
    {
        if ( nReturn == SQL_NO_DATA )
            return false;

        InterpretErrorCodes(nReturn, m_pOdbcStatement);
        ThrowDatabaseException();
        return false;
    }

    m_fieldValues.Clear();
    for ( int i=0; i<(int)m_FieldLookupMap.size(); i++ )
        m_fieldValues.push_back( wxVariant() );
      
    return true;
}


// get field
bool OdbcResultSet::IsFieldNull(int nField)
{
  // Some ODBC drivers (i.e. MS SQL SERVER) need the fields to be retrieved in order
  for (int ctr = 1; ctr < nField; ctr++)
  {
    if (m_fieldValues[ctr-1].IsNull())
    {
      if (IsBlob(ctr))
      {
        wxMemoryBuffer buffer;
        GetResultBlob(ctr, buffer);
      }
      else
      {
        RetrieveFieldData(ctr);
      }
    }
  }

  if (!IsBlob(nField))
  {
    if (m_RetrievedValues.find(nField) == m_RetrievedValues.end())
    {
      RetrieveFieldData(nField);
    }
  }
  else
  {
    wxMemoryBuffer buffer;
    void* pBlob = GetResultBlob(nField, buffer);
    if (pBlob == NULL)
      m_NullValues.insert(nField);
  }

  return (m_NullValues.find(nField) != m_NullValues.end());
}

int OdbcResultSet::GetFieldLength(int nField)
{
    // Some ODBC drivers (i.e. MS SQL SERVER) need the fields to be retrieved in order
    for (int ctr = 1; ctr <= nField; ctr++)
    {
        if (m_fieldValues[ctr-1].IsNull())
        {
            if (!IsBlob(ctr))
            {
                RetrieveFieldData(ctr);
            }
            else
            {
                wxMemoryBuffer buffer;
                GetResultBlob(ctr, buffer);
            }
        }
    }
    /*
    if (m_fieldValues[nField-1].IsNull())
        RetrieveFieldData(nField);
        */

    wxString strValue = m_fieldValues[nField-1].GetString();

    strValue = strValue.Trim();
    size_t real_size = strValue.Length();

    return real_size;
}

void OdbcResultSet::RetrieveFieldData(int nField)
{
    if (nField != -1)
    {
      SQLRETURN rc;
      SQLSMALLINT buflen;
      unsigned long int colType;
      rc = m_pInterface->GetSQLColAttribute()(m_pOdbcStatement, nField, SQL_DESC_TYPE, NULL, 0,
        &buflen, &colType);

      if (SQL_FLOAT == colType || SQL_DOUBLE == colType)
      {
        SQLFLOAT ret;
        SQLINTEGER sqlPtr;
        rc = m_pInterface->GetSQLGetData()(m_pOdbcStatement, nField, SQL_C_DOUBLE, &ret, 0, &sqlPtr);
        if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        {
          InterpretErrorCodes(rc, m_pOdbcStatement);
          ThrowDatabaseException();
          return;
        }
        // Mark this field as retrieved
        m_RetrievedValues.insert(nField);
        // Record whether this field is NULL
        if (sqlPtr == SQL_NULL_DATA)
        {
          m_NullValues.insert(nField);
          ret = 0;
          m_fieldValues[nField-1] = ret;
        }
        else
        {
          m_fieldValues[nField-1] = ret;
        }
      }
      else if (SQL_DATETIME == colType)
      {
        TIMESTAMP_STRUCT ret;
        SQLINTEGER sqlPtr;
        rc = m_pInterface->GetSQLGetData()(m_pOdbcStatement, nField, SQL_C_TIMESTAMP, &ret, sizeof(ret),
          &sqlPtr);

        if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        {
          InterpretErrorCodes(rc, m_pOdbcStatement);
          ThrowDatabaseException();
          return;
        }
        // Mark this field as retrieved
        m_RetrievedValues.insert(nField);
        // Record whether this field is NULL
        if (sqlPtr == SQL_NULL_DATA)
        {
          m_NullValues.insert(nField);
          //m_fieldValues[nField-1] = wxInvalidDateTime;
        }
        else
        {
          /*
          wxPrintf(_T("day = %d, month = %d, year = %d, hour = %d, minute = %d, second = %d, fraction = %d\n"), 
            ret.day, ret.month, ret.year, ret.hour, ret.minute, ret.second, ret.fraction);*/
          wxDateTime dt(ret.day, wxDateTime::Month(ret.month-1), ret.year, ret.hour,
            ret.minute, ret.second, ret.fraction);
          m_fieldValues[nField-1] = dt;
        }
      }
      else
      {
        wxString strValue;
        SQLPOINTER buff[8192];

        memset(buff, 0, 8192*sizeof(SQLTCHAR));

        SQLINTEGER  col_size         = 8192;
        SQLINTEGER  real_size        = 0;

        if (m_pOdbcStatement == NULL)
            m_pOdbcStatement = m_pStatement->GetLastStatement();

        SQLRETURN nRet = m_pInterface->GetSQLGetData()( m_pOdbcStatement, nField, SQL_C_CHAR, buff,
            col_size, &real_size );
        if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
        {
            InterpretErrorCodes(nRet, m_pOdbcStatement);
            ThrowDatabaseException();
            return;
        }
        strValue += ConvertFromUnicodeStream((const char*)buff);

        // Mark this field as retrieved
        m_RetrievedValues.insert(nField);
        // Record whether this field is NULL
        if (real_size == SQL_NULL_DATA)
          m_NullValues.insert(nField);

        if ( real_size > col_size )
        {
            while ( nRet != SQL_NO_DATA )
            {
                nRet = m_pInterface->GetSQLGetData()( m_pOdbcStatement, nField, SQL_C_CHAR, buff, 
                    col_size, &real_size );
                if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO && nRet != SQL_NO_DATA )
                {
                    InterpretErrorCodes(nRet, m_pOdbcStatement);
                    ThrowDatabaseException();
                    return;
                }
                strValue += ConvertFromUnicodeStream((const char*)buff);
            }
        }

        m_fieldValues[nField-1] = strValue;//.Trim();
      }
    }
}

int OdbcResultSet::GetFieldLength(const wxString& strField)
{
    int nIndex = LookupField(strField);
    if ( nIndex == -1 )
        return -1;

    return GetFieldLength( nIndex );
}

int OdbcResultSet::GetResultInt(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) < 0 )
            return 0;
    }

    return m_fieldValues[nField-1].GetLong();
}

wxString OdbcResultSet::GetResultString(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) < 0 )
            return wxEmptyString;
    }

    return m_fieldValues[nField-1].GetString();
}

long OdbcResultSet::GetResultLong(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) < 0 )
            return 0;
    }

    return m_fieldValues[nField-1].GetLong();
}

bool OdbcResultSet::GetResultBool(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) < 0 )
            return false;
    }

    return m_fieldValues[nField-1].GetBool();
}

wxDateTime OdbcResultSet::GetResultDate(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) <= 0 )
            return wxInvalidDateTime;
    }

    return m_fieldValues[nField-1].GetDateTime();
}

double OdbcResultSet::GetResultDouble(int nField)
{
    if ( m_fieldValues[nField-1].IsNull() )
    {
        if ( GetFieldLength(nField) < 0 )
            return 0;
    }

    return m_fieldValues[nField-1].GetDouble();
}

void* OdbcResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
    if (m_BlobMap.find(nField) == m_BlobMap.end())
    {
      if (m_pOdbcStatement == NULL)
          m_pOdbcStatement = m_pStatement->GetLastStatement();

      if (m_NullValues.find(nField) != m_NullValues.end())
        return NULL;

      SQLINTEGER iLength = 8192;
      SQLINTEGER iSize = 0;
      unsigned char buff[8193];

      memset(buff, 0, 8193*sizeof(unsigned char));

      long nReturn = m_pInterface->GetSQLBindParameter()(m_pOdbcStatement, nField, SQL_PARAM_OUTPUT,
                      SQL_C_BINARY, SQL_BINARY, iLength, 0, &buff, iLength, &iSize);

      // Mark this field as retrieved
      m_RetrievedValues.insert(nField);
      // Record whether this field is NULL
      if (iSize == SQL_NULL_DATA)
      {
        m_NullValues.insert(nField);
        return NULL;
      }

      nReturn = m_pInterface->GetSQLGetData()( m_pOdbcStatement, nField, SQL_C_BINARY, &buff, iLength, &iSize );
      if ( nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO )
      {
          wxLogError(_T("Error with RunQueryWithResults - 1\n"));
          InterpretErrorCodes(nReturn, m_pOdbcStatement);
          ThrowDatabaseException();
      }

      // NULL data
      if (iSize < 0)
      {
        wxMemoryBuffer tempBuffer(0);
        tempBuffer.SetDataLen(0);
        tempBuffer.SetBufSize(0);
        Buffer = tempBuffer;

        // Add null blobs to the map as well
        m_BlobMap[nField] = tempBuffer;
        return NULL;
      }

      size_t dataLength = (iLength < iSize) ? iLength : iSize;
      size_t bufferSize = dataLength;
      wxMemoryBuffer tempBuffer(dataLength);

      tempBuffer.AppendData( buff, dataLength );

      while ( iSize > iLength )
      {
          nReturn = m_pInterface->GetSQLGetData()( m_pOdbcStatement, nField, SQL_C_BINARY, &buff, iLength, &iSize );
          if ( nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO )
          {
              wxLogError(_T("Error with RunQueryWithResults - 2\n"));
              InterpretErrorCodes(nReturn, m_pOdbcStatement);
              ThrowDatabaseException();
          }

          dataLength = (iLength < iSize) ? iLength : iSize;
          tempBuffer.AppendData( buff, dataLength );
          bufferSize += dataLength;
      }

      wxMemoryBuffer tempBufferExactSize(bufferSize);
      void* pData = tempBufferExactSize.GetWriteBuf(bufferSize);
      memcpy(pData, tempBuffer.GetData(), bufferSize);
      tempBufferExactSize.UngetWriteBuf(bufferSize);
      tempBufferExactSize.SetDataLen(bufferSize);
      tempBufferExactSize.SetBufSize(bufferSize);
      Buffer = tempBufferExactSize;
 
      wxMemoryBuffer localCopy(Buffer);
      m_BlobMap[nField] = localCopy;

      return Buffer.GetData();
    }
    else
    {
      BlobMap::iterator it = m_BlobMap.find(nField);
      if (it == m_BlobMap.end())
      {
        wxMemoryBuffer tempBuffer(0);
        tempBuffer.SetDataLen(0);
        tempBuffer.SetBufSize(0);
        Buffer = tempBuffer;
        return NULL;
      }
      else
      {
        Buffer = it->second;
        return Buffer.GetData();
      }
    }
}

int OdbcResultSet::LookupField(const wxString& strField)
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
        return ((*SearchIterator).second+1); // Add +1 to make the result set 1-based rather than 0-based
    }
}

ResultSetMetaData* OdbcResultSet::GetMetaData()
{
  ResultSetMetaData* pMetaData = new OdbcResultSetMetaData(m_pInterface, m_pOdbcStatement);
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}

void OdbcResultSet::InterpretErrorCodes( long nCode, SQLHSTMT stmth_ptr )
{
  //if ((nCode != SQL_SUCCESS) ) // && (nCode != SQL_SUCCESS_WITH_INFO))
  {
    SQLINTEGER iNativeCode;
    SQLTCHAR strState[ERR_STATE_LEN];
    SQLTCHAR strBuffer[ERR_BUFFER_LEN];
    SQLSMALLINT iMsgLen;

    memset(strState, 0, ERR_STATE_LEN*sizeof(SQLTCHAR));
    memset(strBuffer, 0, ERR_BUFFER_LEN*sizeof(SQLTCHAR));

    m_pInterface->GetSQLGetDiagRec()(SQL_HANDLE_STMT, stmth_ptr, 1, strState, &iNativeCode, 
      strBuffer, ERR_BUFFER_LEN, &iMsgLen);  
 
    SetErrorCode((int)iNativeCode);
    //SetErrorMessage(ConvertFromUnicodeStream((char*)strBuffer));
    SetErrorMessage(wxString((wxChar*)strBuffer));
  }
}

bool OdbcResultSet::IsBlob(int nField)
{
  SQLTCHAR      col_name[8192];
  SQLSMALLINT   col_name_length;
  SQLSMALLINT   col_data_type;
  SQLUINTEGER   col_size;
  SQLSMALLINT   col_decimal_digits;
  SQLSMALLINT   col_nullable;

  memset(col_name, 0, 8192);

  SQLRETURN nRet = m_pInterface->GetSQLDescribeCol()( m_pOdbcStatement, nField, col_name, 
      8192, &col_name_length, &col_data_type, &col_size, &col_decimal_digits, &col_nullable );

  return (col_data_type == SQL_BIT || col_data_type == SQL_BINARY || 
    col_data_type == SQL_VARBINARY || col_data_type == SQL_LONGVARBINARY);
}

