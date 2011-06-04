#ifndef __SQLITE_RESULT_SET_H__
#define __SQLITE_RESULT_SET_H__

#include "DatabaseResultSet.h"

#include "sqlite3.h"

class SqlitePreparedStatement;
class ResultSetMetaData;

class SqliteResultSet : public DatabaseResultSet
{
public:
  // ctor
  SqliteResultSet();
  SqliteResultSet(SqlitePreparedStatement* pStatement, bool bManageStatement = false);

  // dtor
  virtual ~SqliteResultSet();
  
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
  
  SqlitePreparedStatement* m_pStatement;
  sqlite3_stmt* m_pSqliteStatement;

  StringToIntMap m_FieldLookupMap;

  bool m_bManageStatement;
};

#endif // __SQLITE_RESULT_SET_H__

