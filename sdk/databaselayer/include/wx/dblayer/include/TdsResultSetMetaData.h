#ifndef __TDS_RESULT_SET_METADATA_H__
#define __TDS_RESULT_SET_METADATA_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ResultSetMetaData.h"
#include "TdsColumnData.h"

#include "tds.h"

WX_DECLARE_OBJARRAY(TdsColumnData, ArrayOfTdsColumns);

class TdsResultSetMetaData : public ResultSetMetaData
{
public:
  // ctor
  TdsResultSetMetaData(TDSRESULTINFO* pInfo);

  virtual int GetColumnType(int i);
  virtual int GetColumnSize(int i);
  virtual wxString GetColumnName(int i);
  virtual int GetColumnCount();
  
private:
  void CreateResultSetMetaData();

  ArrayOfTdsColumns m_Columns;
};

#endif // __TDS_RESULT_SET_METADATA_H__
