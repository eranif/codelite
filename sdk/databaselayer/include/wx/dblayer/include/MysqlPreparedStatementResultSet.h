#ifndef __MYSQL_PREPARED_STATEMENT_RESULT_SET_H__
#define __MYSQL_PREPARED_STATEMENT_RESULT_SET_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/hashmap.h"

#include "DatabaseResultSet.h"
#include "MysqlPreparedStatementParameter.h"
#include "MysqlInterface.h"

#include "mysql.h"

WX_DECLARE_HASH_MAP(int, MysqlPreparedStatementParameter*, wxIntegerHash, wxIntegerEqual, IntToMysqlParameterMap);
  
class MysqlPreparedStatementResultSet : public DatabaseResultSet
{
public:
  // ctor
  MysqlPreparedStatementResultSet(MysqlInterface* pInterface);
  MysqlPreparedStatementResultSet(MysqlInterface* pInterface, MYSQL_STMT* pStatement, bool bManageStatement = false);

  //dtor
  virtual ~MysqlPreparedStatementResultSet();
  
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
  void ClearPreviousData();
  MYSQL_BIND* GetResultBinding(int nField);

  MysqlInterface* m_pInterface; 
  MYSQL_STMT* m_pStatement;
  MYSQL_BIND* m_pResultBindings;
  
  StringToIntMap m_FieldLookupMap;

  IntToMysqlParameterMap m_BindingWrappers;

  bool m_bManageStatement;
};

#endif // __MYSQL_PREPARED_STATEMENT_RESULT_SET_H__

