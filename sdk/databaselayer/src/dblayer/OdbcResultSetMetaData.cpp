#include "../include/OdbcResultSetMetaData.h"
#include "../include/OdbcDatabaseLayer.h"

// ctor
OdbcResultSetMetaData::OdbcResultSetMetaData(OdbcInterface* pInterface, SQLHSTMT sqlOdbcStatement)
{
  m_pInterface = pInterface;
  m_pOdbcStatement = sqlOdbcStatement;
}

int OdbcResultSetMetaData::GetColumnType(int i)
{
  int returnType = COLUMN_UNKNOWN;

  SQLTCHAR      col_name[8192];
  SQLSMALLINT   col_name_length;
  SQLSMALLINT   col_data_type;
  SQLUINTEGER   col_size;
  SQLSMALLINT   col_decimal_digits;
  SQLSMALLINT   col_nullable;

  memset(col_name, 0, 8192);

  SQLRETURN nRet = m_pInterface->GetSQLDescribeCol()( m_pOdbcStatement, i, col_name, 
      8192, &col_name_length, &col_data_type, &col_size, &col_decimal_digits, &col_nullable );

  if ( nRet != SQL_SUCCESS )
      return returnType;

  switch (col_data_type)
  {
    case SQL_SMALLINT:
    case SQL_INTEGER:
    case SQL_TINYINT:
    case SQL_BIGINT:
      returnType = COLUMN_INTEGER;
      break;
    case SQL_DECIMAL:
    case SQL_NUMERIC:
    case SQL_REAL:
    case SQL_FLOAT:
    case SQL_DOUBLE:
      returnType = COLUMN_DOUBLE;
      break;
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_LONGVARCHAR:
    case SQL_WCHAR:
    case SQL_WVARCHAR:
    case SQL_WLONGVARCHAR:
      returnType = COLUMN_STRING;
      break;
    case SQL_BIT:
    case SQL_BINARY:
    case SQL_VARBINARY:
    case SQL_LONGVARBINARY:
      returnType = COLUMN_BLOB;
      break;
    case SQL_TYPE_DATE:
    case SQL_TYPE_TIME:
    case SQL_TYPE_TIMESTAMP:
        returnType = COLUMN_DATE;
        break;
    default:
      returnType = COLUMN_UNKNOWN;
      break;
  };

  return returnType;
}

int OdbcResultSetMetaData::GetColumnSize(int i)
{
  int returnSize = -1;

  SQLTCHAR      col_name[8192];
  SQLSMALLINT   col_name_length;
  SQLSMALLINT   col_data_type;
  SQLUINTEGER   col_size;
  SQLSMALLINT   col_decimal_digits;
  SQLSMALLINT   col_nullable;

  memset(col_name, 0, 8192);

  SQLRETURN nRet = m_pInterface->GetSQLDescribeCol()( m_pOdbcStatement, i, col_name, 
      8192, &col_name_length, &col_data_type, &col_size, &col_decimal_digits, &col_nullable );

  if ( nRet == SQL_SUCCESS )
      return col_size;

  return returnSize;
}

wxString OdbcResultSetMetaData::GetColumnName(int i)
{
  wxString columnName;

  SQLTCHAR      col_name[8192];
  SQLSMALLINT   col_name_length;
  SQLSMALLINT   col_data_type;
  SQLUINTEGER   col_size;
  SQLSMALLINT   col_decimal_digits;
  SQLSMALLINT   col_nullable;

  memset(col_name, 0, 8192);

  SQLRETURN nRet = m_pInterface->GetSQLDescribeCol()( m_pOdbcStatement, i, col_name, 
      8192, &col_name_length, &col_data_type, &col_size, &col_decimal_digits, &col_nullable );

  if ( nRet == SQL_SUCCESS )
  {
      //return wxString((wxChar*)col_name);
	  columnName = ConvertFromUnicodeStream((const char*)(wxChar*)col_name);
  }

  return columnName;
}

int OdbcResultSetMetaData::GetColumnCount()
{
  SQLSMALLINT   col_count;

  SQLRETURN nRet = m_pInterface->GetSQLNumResultCols()( m_pOdbcStatement, &col_count );
  
  if ( nRet == SQL_SUCCESS )
      return col_count;
  
  return 0;
}


