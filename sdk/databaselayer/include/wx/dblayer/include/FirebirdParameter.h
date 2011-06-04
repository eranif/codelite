#ifndef __FIREBIRD_PARAMETER_H__
#define __FIREBIRD_PARAMETER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/datetime.h"

#include "DatabaseStringConverter.h"
#include "FirebirdInterface.h"

#include "ibase.h"

class FirebirdParameter : public DatabaseStringConverter
{
public:
  // ctor
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, const wxString& strValue, const wxCSConv* conv);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, int nValue);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, double dblValue);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, bool bValue);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, const wxDateTime& dateValue);
  FirebirdParameter(FirebirdInterface* pInterface, XSQLVAR* pVar, isc_db_handle pDatabase, isc_tr_handle pTransaction, const void* pData, long nDataLength);

  // dtor
  virtual ~FirebirdParameter();

  enum {
    PARAM_STRING = 0,
    PARAM_INT,
    PARAM_DOUBLE,
    PARAM_DATETIME,
    PARAM_BOOL,
    PARAM_BLOB,
    PARAM_NULL
  };
   
  long unsigned int GetDataLength();
  long unsigned int* GetDataLengthPtr();
  
  const void* GetDataPtr();
  int GetParameterType();

  short GetBufferType();

  const XSQLVAR* GetFirebirdSqlVarPtr() { return m_pParameter; }
  void ResetBlob();

private:
  int m_nParameterType;
  
  // A union would probably be better here
  wxString m_strValue;
  int m_nValue;
  float m_fValue;
  double m_dblValue;
  ISC_TIMESTAMP m_Date;
  bool m_bValue;
  wxMemoryBuffer m_BufferValue;
  long unsigned int m_nBufferLength;
  short m_nNullFlag;
  ISC_QUAD m_BlobId;
  isc_blob_handle m_pBlob;
  isc_db_handle m_pDatabase;
  isc_tr_handle m_pTransaction;
  
  XSQLVAR* m_pParameter;
  FirebirdInterface* m_pInterface;
};

#endif // __FIREBIRD_PARAMETER_H__

