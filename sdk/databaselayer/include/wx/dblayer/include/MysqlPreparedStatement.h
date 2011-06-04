#ifndef __MYSQL_PREPARED_STATEMENT_H__
#define __MYSQL_PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynarray.h"

#include "PreparedStatement.h"
#include "MysqlPreparedStatementParameter.h"
#include "MysqlPreparedStatementWrapper.h"
#include "MysqlInterface.h"

#include "mysql.h"

class DatabaseResultSet;

WX_DEFINE_ARRAY_PTR(MysqlPreparedStatementWrapper*, MysqlStatementWrapperArray);
    
class MysqlPreparedStatement : public PreparedStatement
{
public:
  // ctor
  MysqlPreparedStatement(MysqlInterface* pInterface);
  MysqlPreparedStatement(MysqlInterface* pInterface, MYSQL_STMT* pStatement);

  // dtor
  virtual ~MysqlPreparedStatement();

  virtual void Close();
  
  void AddPreparedStatement(MYSQL_STMT* pStatement);
  
  // get field
  virtual void SetParamInt(int nPosition, int nValue);
  virtual void SetParamDouble(int nPosition, double dblValue);
  virtual void SetParamString(int nPosition, const wxString& strValue);
  virtual void SetParamNull(int nPosition);
  virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength);
  virtual void SetParamDate(int nPosition, const wxDateTime& dateValue);
  virtual void SetParamBool(int nPosition, bool bValue);
  virtual int GetParameterCount();
  
  virtual int RunQuery();
  virtual DatabaseResultSet* RunQueryWithResults();

private:
  int FindStatementAndAdjustPositionIndex(int* pPosition);

  MysqlInterface* m_pInterface;
  MysqlStatementWrapperArray m_Statements;
};

#endif // __MYSQL_PREPARED_STATEMENT_H__

