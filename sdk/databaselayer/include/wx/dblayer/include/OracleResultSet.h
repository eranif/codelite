#ifndef __ORACLE_RESULT_SET_H__
#define __ORACLE_RESULT_SET_H__

#include "DatabaseResultSet.h"

#include "occi.h"
#include <vector>

class OracleResultSet : public DatabaseResultSet
{
public:
  // ctor
  OracleResultSet(oracle::occi::ResultSet* pResultSet, bool bManageStatement = false);
  
  // dtor
  virtual ~OracleResultSet();

  virtual bool Next();
  virtual void Close();
  
  virtual int LookupField(const wxString& strField);

  // get field
  virtual int GetResultInt(int nField);
  virtual wxString GetResultString(int nField);
  virtual long GetResultLong(int nField);
  virtual bool GetResultBool(int nField);
  virtual wxDateTime GetResultDate(int nField);
  virtual void* GetResultBlob(int nrField, wxMemoryBuffer& Buffer);
  virtual double GetResultDouble(int nField);
  virtual bool IsFieldNull(int nField);

  virtual ResultSetMetaData* GetMetaData();
  
private:
  StringToIntMap m_FieldLookupMap;
  
  oracle::occi::ResultSet* m_pResultSet;
  std::vector<oracle::occi::MetaData> m_MetaData;

  bool m_bManageStatement;
};

#endif //__ORACLE_RESULT_SET_H__

