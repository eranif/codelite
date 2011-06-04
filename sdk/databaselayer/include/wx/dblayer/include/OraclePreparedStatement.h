#ifndef __ORACLE_PREPARED_STATEMENT_H__
#define __ORACLE_PREPARED_STATEMENT_H__

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
#include "OraclePreparedStatementWrapper.h"

#include "occi.h"

//WX_DEFINE_ARRAY_PTR(oracle::occi::Statement*, OracleStatementVector);
WX_DEFINE_ARRAY_PTR(OraclePreparedStatementWrapper*, OracleStatementVector);

class OraclePreparedStatement : public PreparedStatement
{
public:
  // ctor
  OraclePreparedStatement(oracle::occi::Environment* pEnvironment);
  //OraclePreparedStatement(oracle::occi::Environment* pEnvironment, oracle::occi::Statement* pStatement);
  //OraclePreparedStatement(oracle::occi::Environment* pEnvironment, const wxString& strSQL);

  // dtor
  virtual ~OraclePreparedStatement();

  virtual void Close();

  //static wxString TranslateSQL(const wxString& strOriginalSQL);
  //void AddStatement(oracle::occi::Statement* pStatement);
  void AddStatement(OraclePreparedStatementWrapper* pStatement);

  // get field
  virtual void SetParamInt(int nPosition, int nValue);
  virtual void SetParamDouble(int nPosition, double dblValue);
  virtual void SetParamString(int nPosition, const wxString& strValue);
  virtual void SetParamNull(int nPosition);
  virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength);
  virtual void SetParamDate(int nPosition, const wxDateTime& dateValue);
  virtual void SetParamBool(int nPosition, bool bValue);
  virtual int GetParameterCount();

  virtual void RunQuery();
  virtual DatabaseResultSet* RunQueryWithResults();

private:
  int FindStatementAndAdjustPositionIndex(int* pPosition);

  OracleStatementVector m_Statements;
  oracle::occi::Environment* m_pEnvironment;  // Needed to create Date objects
};

#endif // __ORACLE_PREPARED_STATEMENT_H__

