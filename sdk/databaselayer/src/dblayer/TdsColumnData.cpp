#include "../include/TdsColumnData.h"
#include "../include/ResultSetMetaData.h"

#include "tds.h"

TdsColumnData::TdsColumnData(TDSCOLUMN* pColumn)
{
  m_nColumnType = GetColumnType(pColumn);
  m_nColumnSize = GetColumnSize(pColumn);
  m_strColumnName = GetColumnName(pColumn);
}

int TdsColumnData::GetColumnType(TDSCOLUMN* pColumn)
{
  int returnType = ResultSetMetaData::COLUMN_UNKNOWN;
  switch (pColumn->column_type)
  {
    case SYBSINT1:
    //case SYBSINT2:
    //case SYBSINT4:
    //case SYBSINT8:
    //case SYBSUINT1:
    //case SYBSUINT2:
    //case SYBSUINT4:
    //case SYBSUINT8:
    case SYBINTN:
    case SYBNUMERIC:
      returnType = ResultSetMetaData::COLUMN_INTEGER;
      break;
    case SYBDECIMAL:
    case SYBFLTN:
    case SYBMONEY:
    case SYBMONEYN:
      returnType = ResultSetMetaData::COLUMN_DOUBLE;
      break;
    case SYBVARCHAR:
    case XSYBVARCHAR:
    case XSYBNVARCHAR:
      returnType = ResultSetMetaData::COLUMN_STRING;
      break;
    case SYBVARBINARY:
    case SYBBINARY:
    case SYBIMAGE:
      returnType = ResultSetMetaData::COLUMN_BLOB;
      break;
    case SYBDATETIME:
    case SYBDATETIMN:
      returnType = ResultSetMetaData::COLUMN_DATE;
      break;
    default:
      returnType = ResultSetMetaData::COLUMN_UNKNOWN;
      break;
  };

  return returnType;
}

int TdsColumnData::GetColumnSize(TDSCOLUMN* pColumn)
{
  return pColumn->column_size;
}

wxString TdsColumnData::GetColumnName(TDSCOLUMN* pColumn)
{
  // We might consider a version of ConvertFromUnicodeStream that excepts the string length
  //  parameter
  wxString colName((wxChar*)pColumn->column_name, pColumn->column_namelen);
  return colName;
}

