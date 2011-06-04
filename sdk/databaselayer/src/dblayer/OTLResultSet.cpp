#include "OTLResultSet.h"
#include "OTLResultSetMetaData.h"
#include "OTLDatabaseLayer.h"
#include "DatabaseLayerException.h"
#include "DatabaseErrorCodes.h"
#include "ResultSetMetaData.h"

// ctor
OTLResultSet::OTLResultSet(otl_stream *otlstream, bool bManageStatement /*= false*/)
{
  try
  {
    m_pResultSet = otlstream;
    m_bManageStatement = bManageStatement;
    m_readIt.attach(*m_pResultSet);
    otl_column_desc* otldesc = m_pResultSet->describe_select(m_colCount);
    int nCounter = 1;
    for (int i = 0; i < m_colCount; i++)
    {
      otl_column_desc *newItem = new otl_column_desc();
      *newItem = otldesc[i];
      m_otldesc.push_back(newItem);
      wxString strField = ConvertFromUnicodeStream(otldesc[i].name);
      m_FieldLookupMap[strField.Upper()] = nCounter;
      nCounter++;
    }
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

// dtor
OTLResultSet::~OTLResultSet()
{
  Close();
}

bool OTLResultSet::Next()
{
  try
  {
    return (m_readIt.next_row());
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return false;
}

void OTLResultSet::Close()
{
  try
  {
    CloseMetaData();

    for (unsigned int i = 0 ; i < m_otldesc.size() ;++i)
    {
      delete m_otldesc[i];
    }
    m_otldesc.clear();

    if (m_pResultSet != NULL)
    {
      delete m_pResultSet;
      m_pResultSet =NULL;
    }
    //fixme
    /*
      otl_stream* pStatement = m_pResultSet->getStatement();
     
       if (pStatement)
       {
         pStatement->closeResultSet(m_pResultSet);
     
         if (m_bManageStatement)
         {
           OTL::occi::Connection* pDatabase = pStatement->getConnection();
           if (pDatabase)
             pDatabase->terminateStatement(pStatement);
         }
       }
    */
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
}

wxString OTLResultSet::GetFieldAsString(int nField)
{
  ResultSetMetaData *metaData = GetMetaData();
  wxString Result;
  switch(metaData->GetColumnType(nField))
  {
  case ResultSetMetaData::COLUMN_INTEGER:
    {
      long lValue = GetResultLong(nField);
      Result = wxString::Format(_("%u"),lValue);
      break;
    }
  case ResultSetMetaData::COLUMN_DOUBLE:
    {
      double dValue = GetResultDouble(nField);
      Result = wxString::Format(_("%f"),dValue);
      break;
    }
  case ResultSetMetaData::COLUMN_DATE:
    {
      wxDateTime dateValue = GetResultDate(nField);
      if (dateValue.IsValid())
        Result = dateValue.Format();
      else
        Result = _("Invalid Date");
      break;
    }
  case ResultSetMetaData::COLUMN_STRING:
    {
      std::string strV;
      m_readIt.get(nField,strV);
      Result = ConvertFromUnicodeStream(strV.c_str());
      break;
    }
  default:
    Result = _("");
    break;
  }

  this->CloseMetaData(metaData);
  wxPrintf(_("Result as string: '%s'\n"), Result.c_str());
  return Result;
}

bool OTLResultSet::GetFieldAsBool(int nField)
{
  return false;
}


// get field
int OTLResultSet::GetResultInt(int nField)
{
  int nValue = -1;
  try
  {
    m_readIt.get(nField,nValue);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return nValue;
}

wxString OTLResultSet::GetResultString(int nField)
{
  wxString strValue = wxEmptyString;
  try
  {
    std::string strV;
    m_readIt.get(nField,strV);
    strValue = ConvertFromUnicodeStream(strV.c_str());
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return strValue;
}

long OTLResultSet::GetResultLong(int nField)
{
  long lValue = -1L;
  try
  {
    m_readIt.get(nField,lValue);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return lValue;
}

bool OTLResultSet::GetResultBool(int nField)
{
  bool bValue = false;
  try
  {
    int nValue = 0;
    m_readIt.get(nField,nValue);
    bValue = (nValue != 0);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return bValue;
}

wxDateTime OTLResultSet::GetResultDate(int nField)
{
  wxDateTime date = wxInvalidDateTime;
  try
  {
    //int nType = m_MetaData[nField-1].getInt(MetaData::ATTR_DATA_TYPE);
    //if (nType = OCCI_SQLT_DAT)
    //{
    //}
    otl_datetime dtime;
    m_readIt.get(nField,dtime);
    int nDay = 0,nMonth = 0,nYear = 0,nHour = 0,nMin = 0,nSecond = 0;

    if (dtime.day >= 0 )
      nDay = dtime.day;

    if (dtime.month > 0 )
      nMonth = dtime.month;

    if (dtime.year >= 0 )
      nYear = dtime.year;

    if (dtime.hour >= 0 )
      nHour = dtime.hour;

    if (dtime.minute >= 0 )
      nMin = dtime.minute;

    if (dtime.second >= 0 )
      nSecond = dtime.second;

    if (nDay == 0 && nMonth == 0 && nYear == 0 && nHour == 0 && nMin == 0 && nSecond == 0)
      date = wxInvalidDateTime;
    else
      date.Set(nDay,wxDateTime::Month(nMonth-1),nYear,nHour,nMin,nSecond);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return date;
}

void* OTLResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  try
  {
    /*fixme
       OTL::occi::Blob blob = m_pResultSet->getBlob(nField);
       blob.open(OTL::occi::OCCI_LOB_READONLY);
       int nLength = blob.length();
       unsigned char* pBuffer = (unsigned char*)(Buffer.GetWriteBuf(nLength));
       int nLengthRead = blob.read(nLength, pBuffer, nLength);
    */
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return Buffer.GetData();
}

double OTLResultSet::GetResultDouble(int nField)
{
  double dblValue = -1;
  try
  {
    m_readIt.get(nField,dblValue);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return dblValue;
}

bool OTLResultSet::IsFieldNull(int nField)
{
  try
  {

    return m_readIt.is_null(nField+1);
  }
  catch (otl_exception& e)
  {
    SetErrorCode(OTLDatabaseLayer::TranslateErrorCode(e.code));
    SetErrorMessage(ConvertFromUnicodeStream((char*)e.msg));
    ThrowDatabaseException();
  }
  return true;
}

int OTLResultSet::LookupField(const wxString& strField)
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
    return (*SearchIterator).second;
  }
}

ResultSetMetaData* OTLResultSet::GetMetaData()
{  
  OTLResultSetMetaData* pMetaData =  new OTLResultSetMetaData(m_otldesc);
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}

