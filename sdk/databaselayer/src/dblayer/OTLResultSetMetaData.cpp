#include "OTLResultSetMetaData.h"
#include <vector>
#include <sql.h>
#include <sqlext.h>

// ctor
OTLResultSetMetaData::OTLResultSetMetaData(std::vector<otl_column_desc*> &desc)
{
  for (unsigned int i =0 ;i <desc.size(); ++i )
  {
    otl_column_desc* newItem = new otl_column_desc();
    *newItem = *desc[i];
    m_desc.push_back(newItem);
  }
}

OTLResultSetMetaData::~OTLResultSetMetaData()
{
  for (unsigned int i =0 ;i <m_desc.size(); ++i )
  {
    delete m_desc[i];
  }
  m_desc.clear();
}
int OTLResultSetMetaData::GetColumnType(int i)
{
  int fieldType = COLUMN_UNKNOWN;
  int OTLFieldType = m_desc[i]->dbtype;
  wxString strV(ConvertFromUnicodeStream(m_desc[i]->name));


  switch (OTLFieldType)
  {
  case extLong:
  case extUInt:
  case extNumber:
  case extInt:
  case extVarNum:
  case extRowId: //fixme: check
    fieldType = COLUMN_INTEGER;
    break;

  case extVarChar2:
  case extLongVarChar:
  case extCChar:
  case extVarChar:
  case extChar:
  case extCharZ:
  case extMslabel://fixme : Check
    fieldType = COLUMN_STRING;
    break;

  case extFloat:
    fieldType = COLUMN_DOUBLE;
    break;
    /*
     case SQL_BIT:
       fieldType = COLUMN_BOOL;
       break;
    */
  case extBLOB:
  case extCLOB:
  case extVarRaw: //fixme : Check
    //case extRaw: //fixme : Check
  case extLongRaw://fixme : Check
  case extLongVarRaw://fixme : Check
    fieldType = COLUMN_BLOB;
    break;

  case extDate:
#if (defined(OTL_ORA8I)||defined(OTL_ORA9I))&&defined(OTL_ORA_TIMESTAMP)

  case extTimestamp:
  case extTimestamp_TZ:
  case extTimestamp_LTZ:
#endif

    fieldType = COLUMN_DATE;
    break;

  default:
    fieldType = COLUMN_UNKNOWN;
    break;
  };
  return fieldType;
}

int OTLResultSetMetaData::GetColumnSize(int i)
{
  int OTLFieldSize = m_desc[i]->dbsize;
  return OTLFieldSize;
}

wxString OTLResultSetMetaData::GetColumnName(int i)
{
  wxString strField = ConvertFromUnicodeStream(m_desc[i]->name);
  return strField;
}

int OTLResultSetMetaData::GetColumnCount()
{
  return m_desc.size();
}

