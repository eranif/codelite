#ifndef __OTL_RESULT_SET_METADATA_H__
#define __OTL_RESULT_SET_METADATA_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "ResultSetMetaData.h"
#include <vector>

#define OTL_ORA8I // Compile OTL 4/OCI8
#define OTL_STL // Turn on STL features and otl_value<T>
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif

#ifndef OTL_STREAM_READ_ITERATOR_ON
  #define OTL_STREAM_READ_ITERATOR_ON
#endif
#include "otlv4.h"

class OTLResultSetMetaData : public ResultSetMetaData
{
public:
  // ctor
  OTLResultSetMetaData(std::vector<otl_column_desc*> &desc);

  // dtor
  virtual ~OTLResultSetMetaData();

  virtual int GetColumnType(int i);
  virtual int GetColumnSize(int i);
  virtual wxString GetColumnName(int i);
  virtual int GetColumnCount();

private:
  std::vector<otl_column_desc*> m_desc;
};

#endif // __OTL_RESULT_SET_METADATA_H__

