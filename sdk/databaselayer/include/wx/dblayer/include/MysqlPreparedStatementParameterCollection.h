#ifndef __MYSQL_PREPARED_STATEMENT_PARAMETER_COLLECTION_H__
#define __MYSQL_PREPARED_STATEMENT_PARAMETER_COLLECTION_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/dynarray.h"

#include "DatabaseStringConverter.h"
#include "MysqlParameter.h"

WX_DEFINE_ARRAY_PTR(MysqlParameter*, MysqlParameterArray);

class MysqlPreparedStatementParameterCollection : public DatabaseStringConverter
{
public:
  // ctor
  MysqlPreparedStatementParameterCollection();

  // dtor
  virtual ~MysqlPreparedStatementParameterCollection();

  int GetSize();
  MYSQL_BIND* GetMysqlParameterBindings();

  void SetParam(int nPosition, int nValue);
  void SetParam(int nPosition, double dblValue);
  void SetParam(int nPosition, const wxString& strValue);
  void SetParam(int nPosition);
  void SetParam(int nPosition, const void* pData, long nDataLength);
  void SetParam(int nPosition, const wxDateTime& dateValue);
  void SetParam(int nPosition, bool bValue);
  void SetParam(int nPosition, MysqlParameter* pParameter);
  
private:
  MysqlParameterArray m_Parameters;
};

#endif // __MYSQL_PREPARED_STATEMENT_PARAMETER_COLLECTION_H__

