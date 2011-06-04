#ifndef __FIREBIRD_PREPARED_STATEMENT_H__
#define __FIREBIRD_PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ibase.h"

#include "PreparedStatement.h"
#include "FirebirdPreparedStatementWrapper.h"
#include "FirebirdInterface.h"

class DatabaseResultSet;

WX_DEFINE_ARRAY_PTR(FirebirdPreparedStatementWrapper*, FirebirdStatementVector);

class FirebirdPreparedStatement : public PreparedStatement
{
public:
  // ctor
  FirebirdPreparedStatement(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction);

  // dtor
  virtual ~FirebirdPreparedStatement();

  virtual void Close();
  
  void AddPreparedStatement(const wxString& strSQL);
  
  // get field
  virtual void SetParamInt(int nPosition, int nValue);
  virtual void SetParamDouble(int nPosition, double dblValue);
  virtual void SetParamString(int nPosition, const wxString& strValue);
  virtual void SetParamNull(int nPosition);
  virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength);
  virtual void SetParamDate(int nPosition, const wxDateTime& date);
  virtual void SetParamBool(int nPosition, bool bValue);
  virtual int GetParameterCount();
  
  virtual int RunQuery();
  virtual DatabaseResultSet* RunQueryWithResults();

  static FirebirdPreparedStatement* CreateStatement(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction, const wxString& strSQL, const wxCSConv* conv);

  void SetManageTransaction(bool bManageTransaction) { m_bManageTransaction = bManageTransaction; }
  //void SetTransaction(isc_tr_handle pTransaction) { m_pTransaction = pTransaction; }

private:
  int FindStatementAndAdjustPositionIndex(int* pPosition);
  void SetInvalidParameterPositionError(int nPosition);
  void InterpretErrorCodes();

  bool m_bManageTransaction;
  isc_tr_handle m_pTransaction;
  isc_db_handle m_pDatabase;

  FirebirdStatementVector m_Statements;
  ISC_STATUS_ARRAY m_Status;
  FirebirdInterface* m_pInterface;
};

#endif // __FIREBIRD_PREPARED_STATEMENT_H__

