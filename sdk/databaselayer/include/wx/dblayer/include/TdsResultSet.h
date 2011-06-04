#ifndef __TDS_RESULT_SET_H__
#define __TDS_RESULT_SET_H__

#include "DatabaseResultSet.h"

#include "tds.h"

class ResultSetMetaData;

class TdsResultSet : public DatabaseResultSet
{
public:
  // ctor
  TdsResultSet(TDSSOCKET* pDatabase);

  // dtor
  virtual ~TdsResultSet();
  
  virtual bool Next();
  virtual void Close();
  
  virtual int LookupField(const wxString& strField);

  // get field
  virtual int GetResultInt(int nField);
  virtual wxString GetResultString(int nField);
  virtual long GetResultLong(int nField);
  virtual bool GetResultBool(int nField);
  virtual wxDateTime GetResultDate(int nField);
  virtual void* GetResultBlob(int nField, wxMemoryBuffer& Buffer);
  virtual double GetResultDouble(int nField);
  virtual bool IsFieldNull(int nField);

  // get MetaData
  virtual ResultSetMetaData* GetMetaData();

private:
  void FreeResultSets();
  void PopulateFieldMap();
  void CreateResultSetMetaData();
  void SetErrorInformationFromDatabaseLayer();

  StringToIntMap m_FieldLookupMap;

  TDSSOCKET* m_pDatabase;
  TDSRESULTINFO* m_pResultInfo;
  ResultSetMetaData* m_pResultSetMetaData;
};

#endif // __TDS_RESULT_SET_H__

