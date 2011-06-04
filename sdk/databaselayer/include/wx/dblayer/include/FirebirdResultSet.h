#ifndef __FIREBIRD_RESULT_SET_H__
#define __FIREBIRD_RESULT_SET_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ibase.h"

#include "DatabaseResultSet.h"
#include "FirebirdInterface.h"

class FirebirdResultSet : public DatabaseResultSet
{
public:
  // ctor
  FirebirdResultSet(FirebirdInterface* pInterface);
  FirebirdResultSet(FirebirdInterface* pInterface, isc_db_handle pDatabase, isc_tr_handle pTransaction, isc_stmt_handle pStatement, XSQLDA* pFields,
      bool bManageStmt = false, bool bManageTrans = false);

  // dtor
  virtual ~FirebirdResultSet();

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
  bool IsNull(XSQLVAR* pVar);
  void SetDateTimeFromTm(wxDateTime& dateReturn, struct tm& timeInTm);
  
  void AllocateFieldSpace();
  void FreeFieldSpace();
  void PopulateFieldLookupMap();
  
  void InterpretErrorCodes();

  StringToIntMap m_FieldLookupMap;

  // The database and transaction handles are needed to retrieve BLOB objects
  isc_db_handle m_pDatabase;
  isc_tr_handle m_pTransaction;

  isc_stmt_handle m_pStatement;
  XSQLDA* m_pFields;

  ISC_STATUS_ARRAY m_Status;
  FirebirdInterface* m_pInterface;

  bool m_bManageStatement;
  bool m_bManageTransaction;
};

#endif // __FIREBIRD_RESULT_SET_H__

