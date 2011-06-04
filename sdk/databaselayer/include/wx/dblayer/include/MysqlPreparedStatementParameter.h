#ifndef __MYSQL_PREPARED_STATEMENT_PARAMETER_H__
#define __MYSQL_PREPARED_STATEMENT_PARAMETER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/datetime.h"
#include "wx/buffer.h"

#include "mysql.h"

#include "DatabaseErrorReporter.h"
#include "DatabaseStringConverter.h"

typedef struct bind_data {
  wxString strValue;
  int nValue;
  double dblValue;
  MYSQL_TIME dateValue;
  bool bValue;
  wxMemoryBuffer bufferValue;
  wxCharBuffer charBufferValue;
  unsigned long nBufferLength;
} MysqlBindData;

class MysqlPreparedStatementParameter : public DatabaseErrorReporter, public DatabaseStringConverter
{
public:
  // ctor
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind);  // Default to NULL
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const wxString& strValue);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, int nValue);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, double dblValue);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const void* pData, long nDataLength);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, const wxDateTime& dateValue);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, bool bValue);
  MysqlPreparedStatementParameter(MYSQL_BIND* pBind, MYSQL_FIELD* pField);

  // dtor
  virtual ~MysqlPreparedStatementParameter();
  
  MYSQL_BIND* GetMysqlBind() { return m_pBind; }
  MysqlBindData* GetBindData() { return &m_Data; }

  void SetInt(int nValue);
  void SetDouble(double dblValue);
  void SetString(const wxString& strValue);
  void SetNull();
  void SetBlob(const void* pData, long nDataLength);
  void SetDate(const wxDateTime& dateValue);
  void SetBool(bool bValue);

  void ClearBuffer();
private:
  MYSQL_BIND* m_pBind;
  MysqlBindData m_Data;
  my_bool m_bIsNull;
};

#endif // __MYSQL_PREPARED_STATEMENT_PARAMETER_H__

