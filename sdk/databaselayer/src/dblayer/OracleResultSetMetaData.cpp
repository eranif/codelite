#include "../include/OracleResultSetMetaData.h"

// ctor
OracleResultSetMetaData::OracleResultSetMetaData(std::vector<oracle::occi::MetaData> metaData)
{
  m_MetaData = metaData;
}

int OracleResultSetMetaData::GetColumnType(int i)
{
  int fieldType = COLUMN_UNKNOWN;
  int oracleFieldType = m_MetaData[i].getInt(oracle::occi::MetaData::ATTR_DATA_TYPE);
  switch (oracleFieldType)
  {
    case oracle::occi::OCCI_SQLT_NUM:
    case oracle::occi::OCCIINT:
    case oracle::occi::OCCI_SQLT_LNG:
    case oracle::occi::OCCINUMBER:
    case oracle::occi::OCCIROWID:
      fieldType = COLUMN_INTEGER;
      break;
    case oracle::occi::OCCI_SQLT_STR:
    case oracle::occi::OCCISTRING:
      fieldType = COLUMN_STRING;
      break;
    case oracle::occi::OCCIFLOAT:
    case oracle::occi::OCCIBFLOAT:
    case oracle::occi::OCCIBDOUBLE:
    case oracle::occi::OCCIIBFLOAT:
    case oracle::occi::OCCIIBDOUBLE:
    case oracle::occi::OCCIDOUBLE:
      fieldType = COLUMN_DOUBLE;
      break;
    case oracle::occi::OCCIBOOL:
      fieldType = COLUMN_BOOL;
      break;
    case oracle::occi::OCCI_SQLT_CLOB:
    case oracle::occi::OCCI_SQLT_BLOB:
    case oracle::occi::OCCIBLOB:
    case oracle::occi::OCCIBYTES:
    case oracle::occi::OCCICLOB:
      fieldType = COLUMN_BLOB;
      break;
    case oracle::occi::OCCI_SQLT_DATE:
    case oracle::occi::OCCI_SQLT_TIME:
    case oracle::occi::OCCI_SQLT_TIME_TZ:
    case oracle::occi::OCCI_SQLT_TIMESTAMP:
    case oracle::occi::OCCI_SQLT_TIMESTAMP_TZ:
    case oracle::occi::OCCI_SQLT_TIMESTAMP_LTZ:
    case oracle::occi::OCCIDATE:
    case oracle::occi::OCCITIMESTAMP:
      fieldType = COLUMN_DATE;
      break;
    default:  
      fieldType = COLUMN_UNKNOWN;
      break;
  };
  return fieldType;
}

int OracleResultSetMetaData::GetColumnSize(int i)
{
  int oracleFieldSize = m_MetaData[i].getInt(oracle::occi::MetaData::ATTR_DATA_SIZE);
  return oracleFieldSize;
}

wxString OracleResultSetMetaData::GetColumnName(int i)
{
  wxString strField = ConvertFromUnicodeStream(m_MetaData[i].getString(oracle::occi::MetaData::ATTR_NAME).c_str());
  return strField;
}

int OracleResultSetMetaData::GetColumnCount()
{
  return m_MetaData.size();
}

