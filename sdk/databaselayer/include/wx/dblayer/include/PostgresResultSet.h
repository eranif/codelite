#ifndef __POSTGRESQL_RESULT_SET_H__
#define __POSTGRESQL_RESULT_SET_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DatabaseResultSet.h"
#include "PostgresInterface.h"

#include "libpq-fe.h"

class PostgresResultSet : public DatabaseResultSet
{
public:
  // ctor
  PostgresResultSet(PostgresInterface* pInterface);
  PostgresResultSet(PostgresInterface* pInterface, PGresult* pResult);

  // dtor
  virtual ~PostgresResultSet();
  
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
  PostgresInterface* m_pInterface; 
  PGresult* m_pResult;

  StringToIntMap m_FieldLookupMap;
  int m_nCurrentRow;
  int m_nTotalRows;
  bool m_bBinaryResults;
};

#endif // __POSTGRESQL_RESULT_SET_H__

