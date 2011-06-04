#ifndef __FIREBIRD_PREPARED_STATEMENT_WRAPPER_H__
#define __FIREBIRD_PREPARED_STATEMENT_WRAPPER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ibase.h"

#include "DatabaseErrorReporter.h"
#include "DatabaseStringConverter.h"
#include "FirebirdParameterCollection.h"
#include "FirebirdInterface.h"

class DatabaseResultSet;

class FirebirdPreparedStatementWrapper : public DatabaseErrorReporter, public DatabaseStringConverter
{
public:
  // ctor
  FirebirdPreparedStatementWrapper(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction, const wxString& strSQL);

  // dtor
  virtual ~FirebirdPreparedStatementWrapper();

  void Prepare(const wxString& strSQL);
  void Prepare();
  
  // set field
  void SetParam(int nPosition, int nValue);
  void SetParam(int nPosition, double dblValue);
  void SetParam(int nPosition, const wxString& strValue);
  void SetParam(int nPosition);
  void SetParam(int nPosition, const void* pData, long nDataLength);
  void SetParam(int nPosition, const wxDateTime& dateValue);
  void SetParam(int nPosition, bool bValue);
  int GetParameterCount();
  
  int RunQuery();
  DatabaseResultSet* RunQueryWithResults();

  void SetTransaction(isc_tr_handle pTransaction) { m_pTransaction = pTransaction; }
  bool IsSelectQuery();

private:
  void InterpretErrorCodes();
  
  wxString m_strSQL;
  isc_stmt_handle m_pStatement;
  XSQLDA* m_pParameters;
  isc_db_handle m_pDatabase;
  isc_tr_handle m_pTransaction;
 
  FirebirdParameterCollection* m_pParameterCollection;

  ISC_STATUS_ARRAY m_Status;
  FirebirdInterface* m_pInterface;

  bool m_bManageStatement;
  bool m_bManageTransaction;
};

#endif // __FIREBIRD_PREPARED_STATEMENT_WRAPPER_H__

