#include "../include/DatabaseQueryParser.h"

bool IsEmptyQuery(const wxString& strQuery)
{
  wxString strQueryCopy(strQuery);
  strQueryCopy.Replace(_(";"), wxT(""));  // Remove all query delimiting semi-colons
  return (strQueryCopy.Trim(true).Trim(false) == wxEmptyString);
}

wxArrayString ParseQueries(const wxString& strQuery)
{
  wxArrayString returnArray;
  bool bInDQuote = false;
  bool bInSQuote = false;
  int nLast = 0;

  for (int i = 0; i < (int) strQuery.Length(); i++) {
    if (!bInDQuote && strQuery.SubString(i, i) == _T("'")) {
        if(!bInSQuote || strQuery.SubString(i - 1, i - 1) != _T("\\")) {
            bInSQuote = !bInSQuote;
        }
            
    }
    else if (!bInSQuote && strQuery.SubString(i, i) == _T("\"")) {
        if(!bInDQuote || strQuery.SubString(i - 1, i - 1) != _T("\\")) {
          bInDQuote = !bInDQuote;
        }
    }
    else if ( strQuery.SubString(i, i) == _T(";") && (!bInSQuote && !bInDQuote)) {
        wxString str;
        str << strQuery.SubString(nLast, i);
        if (!IsEmptyQuery(str))
            returnArray.Add( str );

        nLast = i + 1;
    }
  }

  if ( nLast < (int)strQuery.Length() -1 )
  {
      wxString str;
      str << strQuery.SubString(nLast, strQuery.Length() - 1) << _T(";");
      if (!IsEmptyQuery(str))
        returnArray.Add( str );
  }

  return returnArray;
}

