#include "../include/DatabaseStringConverter.h"

// Default the encoding converter
DatabaseStringConverter::DatabaseStringConverter()
  : m_Encoding(wxT("UTF-8"))
{
}

DatabaseStringConverter::DatabaseStringConverter(const wxChar* charset)
  : m_Encoding(charset)
{
}

void DatabaseStringConverter::SetEncoding(wxFontEncoding encoding)
{
  m_Encoding = wxCSConv(encoding);
}

void DatabaseStringConverter::SetEncoding(const wxCSConv* conv)
{
  m_Encoding = *conv;
}

const wxCharBuffer DatabaseStringConverter::ConvertToUnicodeStream(const wxString& inputString)
{
#if wxUSE_UNICODE
  return wxConvUTF8.cWC2MB(inputString.wc_str(*wxConvCurrent));
#else
  wxString str(inputString.wc_str(*wxConvCurrent), wxConvUTF8);
  return str.mb_str();
#endif
}

size_t DatabaseStringConverter::GetEncodedStreamLength(const wxString& inputString)
{
  if(inputString == _T(""))
    return 0;
#if wxUSE_UNICODE
  size_t length = wxConvUTF8.WC2MB(NULL, inputString.wc_str(), (unsigned int)0);
#else
  wxString str(inputString.wc_str(*wxConvCurrent), wxConvUTF8);
  size_t length = str.Length();
#endif
  if (length == 0)
  {
    wxCharBuffer tempCharBuffer = ConvertToUnicodeStream(inputString);
    length = wxStrlen((wxChar*)(const char*)tempCharBuffer);
  }

  return length;
}

wxString DatabaseStringConverter::ConvertFromUnicodeStream(const char* inputBuffer)
{
  wxString strReturn(wxConvUTF8.cMB2WC(inputBuffer), *wxConvCurrent);
  
  // If the UTF-8 conversion didn't return anything, then try the default unicode conversion
  if (strReturn == wxEmptyString)
    strReturn << wxString(inputBuffer, *wxConvCurrent);
  
  return strReturn;
}

const wxCharBuffer DatabaseStringConverter::ConvertToUnicodeStream(const wxString& inputString, const char* encoding)
{
#if wxUSE_UNICODE
  return wxConvUTF8.cWC2MB(inputString.wc_str(*wxConvCurrent));
#else
  wxString str(inputString.wc_str(*wxConvCurrent), wxConvUTF8);
  return str.mb_str();
#endif
}

size_t DatabaseStringConverter::GetEncodedStreamLength(const wxString& inputString, const char* encoding)
{
  if(inputString == _T(""))
    return 0;
#if wxUSE_UNICODE
  size_t length = wxConvUTF8.WC2MB(NULL, inputString.wc_str(), (unsigned int)0);
#else
  const wchar_t* str = inputString.wc_str(*wxConvCurrent);
  size_t length = wxConvUTF8.WC2MB(NULL, str, (unsigned int)0);
#endif
  if (length == 0)
  {
    wxCharBuffer tempCharBuffer = DatabaseStringConverter::ConvertToUnicodeStream(inputString, encoding);
    length = wxStrlen((wxChar*)(const char*)tempCharBuffer);
  }

  return length;
}

wxString DatabaseStringConverter::ConvertFromUnicodeStream(const char* inputBuffer, const char* encoding)
{
  wxString strReturn(wxConvUTF8.cMB2WC(inputBuffer), *wxConvCurrent);
  
  // If the UTF-8 conversion didn't return anything, then try the default unicode conversion
  if (strReturn == wxEmptyString)
    strReturn << wxString(inputBuffer, *wxConvCurrent);
  
  return strReturn;
}  

