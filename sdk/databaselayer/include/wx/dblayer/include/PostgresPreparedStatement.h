#ifndef __POSTGRESQL_PREPARED_STATEMENT_H__
#define __POSTGRESQL_PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/arrstr.h"
#include "wx/dynarray.h"

#include "PreparedStatement.h"
#include "PostgresResultSet.h"
#include "PostgresPreparedStatementWrapper.h"
#include "PostgresInterface.h"

WX_DECLARE_OBJARRAY(PostgresPreparedStatementWrapper, ArrayOfPostgresPreparedStatementWrappers);

class PostgresPreparedStatement : public PreparedStatement
{
public:
  // ctor
  PostgresPreparedStatement(PostgresInterface* pInterface);
  PostgresPreparedStatement(PostgresInterface* pInterface, PGconn* pDatabase, const wxString& strSQL, const wxString& strStatementName);

  // dtor
  virtual ~PostgresPreparedStatement();

  virtual void Close();

  void AddStatement(PGconn* pDatabase, const wxString& strSQL, const wxString& strStatementName);
  static PostgresPreparedStatement* CreateStatement(PostgresInterface* pInterface, PGconn* pDatabase, const wxString& strSQL);
  
  // set field
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

  static wxString TranslateSQL(const wxString& strOriginalSQL);

private:
  PostgresInterface* m_pInterface;
  int FindStatementAndAdjustPositionIndex(int* pPosition);
  static wxString GenerateRandomStatementName();

  ArrayOfPostgresPreparedStatementWrappers m_Statements;
};

#endif // __POSTGRESQL_PREPARED_STATEMENT_H__

