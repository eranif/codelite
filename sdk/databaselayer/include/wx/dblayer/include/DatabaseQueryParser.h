#ifndef _DATABASEQUERYPARSER_H_
#define _DATABASEQUERYPARSER_H_

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

wxArrayString ParseQueries(const wxString& strQuery);

#endif // _DATABASEQUERYPARSER_H_
