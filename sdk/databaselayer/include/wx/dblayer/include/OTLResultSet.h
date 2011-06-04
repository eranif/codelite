#ifndef __OTL_RESULT_SET_H__
#define __OTL_RESULT_SET_H__

#include "DatabaseResultSet.h"

#define OTL_ORA8I // Compile OTL 4/OCI8
#define OTL_STL // Turn on STL features and otl_value<T>
#ifndef OTL_ANSI_CPP
#define OTL_ANSI_CPP // Turn on ANSI C++ typecasts
#endif

#ifndef OTL_STREAM_READ_ITERATOR_ON
  #define OTL_STREAM_READ_ITERATOR_ON
#endif

#include "otlv4.h"
#include <vector>

class OTLResultSet : public DatabaseResultSet
{
public:
  // ctor
  OTLResultSet(otl_stream *otlstream,bool bManageStatement = false);

  // dtor
  virtual ~OTLResultSet();

  virtual bool Next();
  virtual void Close();

  virtual int LookupField(const wxString& strField);

  // get field
  virtual int GetResultInt(int nField);
  virtual wxString GetResultString(int nField);
  virtual long GetResultLong(int nField);
  virtual bool GetResultBool(int nField);
  virtual wxDateTime GetResultDate(int nField);
  virtual void* GetResultBlob(int nrField, wxMemoryBuffer& Buffer);
  virtual double GetResultDouble(int nField);
  virtual bool IsFieldNull(int nField);

  virtual ResultSetMetaData* GetMetaData();
private:
  wxString GetFieldAsString(int nField);
  bool GetFieldAsBool(int nField);

private:
  StringToIntMap m_FieldLookupMap;
  otl_stream *m_pResultSet;
  otl_stream_read_iterator<otl_stream,otl_exception,otl_lob_stream> m_readIt;
  std::vector<otl_column_desc *> m_otldesc;
  int m_colCount;
  bool m_bManageStatement;
};

#endif //__OTL_RESULT_SET_H__

