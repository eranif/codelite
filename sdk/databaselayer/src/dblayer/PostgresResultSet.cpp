#include "../include/PostgresResultSet.h"
#include "../include/PostgresResultSetMetaData.h"
#include "../include/DatabaseErrorCodes.h"
#include "../include/DatabaseLayer.h"
#include "../include/DatabaseLayerException.h"
#include "libpq/libpq-fs.h"

PostgresResultSet::PostgresResultSet(PostgresInterface* pInterface)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pResult = NULL;
  m_FieldLookupMap.clear();
  m_nCurrentRow = -1;
  m_nTotalRows = 0;
  m_bBinaryResults = false;
}

PostgresResultSet::PostgresResultSet(PostgresInterface* pInterface, PGresult* pResult)
 : DatabaseResultSet()
{
  m_pInterface = pInterface;
  m_pResult = pResult;
  m_nCurrentRow = -1;
  m_nTotalRows = m_pInterface->GetPQntuples()(m_pResult);
  m_bBinaryResults = m_pInterface->GetPQbinaryTuples()(m_pResult);
  
  int nFields = m_pInterface->GetPQnfields()(m_pResult);
  for (int i=0; i<nFields; i++)
  {
    wxString strField = ConvertFromUnicodeStream(m_pInterface->GetPQfname()(pResult, i));
    strField.MakeUpper();
    m_FieldLookupMap[strField] = i;
  }
}

PostgresResultSet::~PostgresResultSet()
{
  Close();
}

bool PostgresResultSet::Next()
{
  if (m_nTotalRows < 1)
    return false;

  m_nCurrentRow++;
  
  return (m_nCurrentRow < m_nTotalRows);
}

void PostgresResultSet::Close()
{
  CloseMetaData();

  if (m_pResult != NULL)
  {
    m_pInterface->GetPQclear()(m_pResult);
    m_pResult = NULL;
  }
  m_FieldLookupMap.clear();
}

  
// get field
int PostgresResultSet::GetResultInt(int nField)
{
  // Don't use nField-1 here since GetResultLong will take care of that
  return GetResultLong(nField);
}

wxString PostgresResultSet::GetResultString(int nField)
{
  wxString strValue = _("");
  if (m_bBinaryResults)
  {
    wxLogError(_("Not implemented\n"));
  }
  else
  {
    if (nField != -1)
    {
      if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
      {
        strValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
      }
    }
  }

  return strValue;
}

long PostgresResultSet::GetResultLong(int nField)
{
  long nValue = 0;
  if (m_bBinaryResults)
  {
    wxLogError(_("Not implemented\n"));
  }
  else
  {
    if (nField != -1)
    {
      if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
      {
        wxString strValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
        strValue.ToLong(&nValue);
      }
    }
  }

  return nValue;
}

bool PostgresResultSet::GetResultBool(int nField)
{
  bool bValue = false;
  if (m_bBinaryResults)
  {
    wxLogError(_("Not implemented\n"));
  }
  else
  {
    if (nField != -1)
    {
      if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
      {
        wxString strValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
        bValue = (strValue != _("0"));
      }
    }
  }

  return bValue;
}

wxDateTime PostgresResultSet::GetResultDate(int nField)
{
  wxDateTime dateValue = wxInvalidDateTime;
  // TIMESTAMP results should be the same in binary or text results
  if (m_bBinaryResults)
  {
    if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
    {
      wxString strDateValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
      if (dateValue.ParseDateTime(strDateValue) == NULL)
      {
        if (dateValue.ParseDate(strDateValue) != NULL)
        {
          dateValue.SetHour(0);
          dateValue.SetMinute(0);
          dateValue.SetSecond(0);
          dateValue.SetMillisecond(0);
        }
        else
        {
          dateValue = wxInvalidDateTime;
        }
      }
    }
  }
  else
  {
    if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
    {
      wxString strDateValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
      if (dateValue.ParseDateTime(strDateValue) == NULL)
      {
        if (dateValue.ParseDate(strDateValue) != NULL)
        {
          dateValue.SetHour(0);
          dateValue.SetMinute(0);
          dateValue.SetSecond(0);
          dateValue.SetMillisecond(0);
        }
        else
        {
          dateValue = wxInvalidDateTime;
        }
      }
    }
  }

  return dateValue;
}

void* PostgresResultSet::GetResultBlob(int nField, wxMemoryBuffer& Buffer)
{
  //int nLength = m_pInterface->GetPQgetlength()(m_pResult, m_nCurrentRow, nIndex);
  unsigned char* pBlob = (unsigned char*)m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1);
  size_t nUnescapedLength = 0;
  unsigned char* pUnescapedBlob = m_pInterface->GetPQunescapeBytea()(pBlob, &nUnescapedLength);

  wxMemoryBuffer tempBuffer(nUnescapedLength);
  void* pUnescapedBuffer = tempBuffer.GetWriteBuf(nUnescapedLength);
  memcpy(pUnescapedBuffer, pUnescapedBlob, nUnescapedLength);
  m_pInterface->GetPQfreemem()(pUnescapedBlob);
  tempBuffer.UngetWriteBuf(nUnescapedLength);

  tempBuffer.SetBufSize(nUnescapedLength);
  tempBuffer.SetDataLen(nUnescapedLength);
  Buffer = tempBuffer;

  Buffer.UngetWriteBuf(nUnescapedLength);

  if (nUnescapedLength < 1)
    return NULL;

  return Buffer.GetData();
}

double PostgresResultSet::GetResultDouble(int nField)
{
  double dblValue = 0;
  if (m_bBinaryResults)
  {
    wxLogError(_("Not implemented\n"));
  }
  else
  {
    if (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) != 1)
    {
      wxString strValue = ConvertFromUnicodeStream(m_pInterface->GetPQgetvalue()(m_pResult, m_nCurrentRow, nField-1));
      strValue.ToDouble(&dblValue);
    }
  }

  return dblValue;
}

bool PostgresResultSet::IsFieldNull(int nField)
{
  return (m_pInterface->GetPQgetisnull()(m_pResult, m_nCurrentRow, nField-1) == 1);
}

int PostgresResultSet::LookupField(const wxString& strField)
{
  wxString strLocalCopy(strField);
  strLocalCopy.MakeUpper();
  StringToIntMap::iterator SearchIterator = m_FieldLookupMap.find(strLocalCopy);
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

ResultSetMetaData* PostgresResultSet::GetMetaData()
{
  ResultSetMetaData* pMetaData = new PostgresResultSetMetaData(m_pInterface, m_pResult);
  LogMetaDataForCleanup(pMetaData);
  return pMetaData;
}

