#ifndef __DATABASE_STRING_CONVERTER_H__
#define __DATABASE_STRING_CONVERTER_H__

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "DatabaseLayerDef.h"

class WXDLLIMPEXP_DATABASELAYER DatabaseStringConverter
{
public:
  // ctor
  DatabaseStringConverter();
  DatabaseStringConverter(const wxChar* charset);

  // dtor
  virtual ~DatabaseStringConverter() { }
  
  void SetEncoding(wxFontEncoding encoding);
  void SetEncoding(const wxCSConv* conv);
  const wxCSConv* GetEncoding() { return &m_Encoding; }

  virtual const wxCharBuffer ConvertToUnicodeStream(const wxString& inputString);
  virtual size_t GetEncodedStreamLength(const wxString& inputString);
  virtual wxString ConvertFromUnicodeStream(const char* inputBuffer);
 
  static const wxCharBuffer ConvertToUnicodeStream(const wxString& inputString, const char* encoding);
  static wxString ConvertFromUnicodeStream(const char* inputBuffer, const char* encoding);
  static size_t GetEncodedStreamLength(const wxString& inputString, const char* encoding);
 
private:
  wxCSConv m_Encoding;
};

#endif // __DATABASE_STRING_CONVERTER_H__
