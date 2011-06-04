#ifndef __POSTGRESQL_PARAMETER_H__
#define __POSTGRESQL_PARAMETER_H__

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

class PostgresParameter : public DatabaseStringConverter
{
public:
  // ctor
  PostgresParameter();
  PostgresParameter(const wxString& strValue);
  PostgresParameter(int nValue);
  PostgresParameter(double dblValue);
  PostgresParameter(bool bValue);
  PostgresParameter(const wxDateTime& dateValue);
  PostgresParameter(const void* pData, long nDataLength);

  // dtor
  virtual ~PostgresParameter() { }

  enum {
    PARAM_STRING = 0,
    PARAM_INT,
    PARAM_DOUBLE,
    PARAM_DATETIME,
    PARAM_BOOL,
    PARAM_BLOB,
    PARAM_NULL
  };
   
  long GetDataLength();
  long* GetDataLengthPointer();
  
  const void* GetDataPtr();
  int GetParameterType();

  bool IsBinary();

private:
  int m_nParameterType;
  
  // A union would probably be better here
  wxString m_strValue;
  int m_nValue;
  double m_dblValue;
  wxString m_strDateValue;
  bool m_bValue;
  wxMemoryBuffer m_BufferValue;
  wxCharBuffer m_CharBufferValue;
  long m_nBufferLength;
  
};


#endif // __POSTGRESQL_PARAMETER_H__
