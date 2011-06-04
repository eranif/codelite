#ifndef __TDS_PREPARED_STATEMENT_H__
#define __TDS_PREPARED_STATEMENT_H__

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

#include "tds.h"

class DatabaseResultSet;

class TdsPreparedStatement : public PreparedStatement
{
public:
  // ctor
  TdsPreparedStatement(TDSSOCKET* pDatabase, const wxString& strQuery);

  // dtor
  virtual ~TdsPreparedStatement();

  virtual void Close();
  
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
  void FreeAllocatedResultSets();
  void AllocateParameter(int nPosition);
  int FindStatementAndAdjustPositionIndex(int* pPosition);
  void SetErrorInformationFromDatabaseLayer();
 
  TDSSOCKET* m_pDatabase;
  TDSDYNAMIC* m_pStatement;
  TDSPARAMINFO* m_pParameters;
  wxString m_strOriginalQuery;
};

#endif // __TDS_PREPARED_STATEMENT_H__

