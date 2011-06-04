#ifndef __TDS_RESULT_SET_COLUMN_METADATA_H__
#define __TDS_RESULT_SET_COLUMN_METADATA_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "tds.h"

class TdsColumnData
{
public:
  // ctor
  TdsColumnData(TDSCOLUMN* pColumn);

  int GetColumnType() { return m_nColumnType; }
  int GetColumnSize() { return m_nColumnSize; }
  wxString GetColumnName() { return m_strColumnName; }

private:
  int GetColumnType(TDSCOLUMN* pColumn);
  int GetColumnSize(TDSCOLUMN* pColumn);
  wxString GetColumnName(TDSCOLUMN* pColumn);

  int m_nColumnType;
  int m_nColumnSize;
  wxString m_strColumnName;
};

#endif // __TDS_RESULT_SET_COLUMN_METADATA_H__

