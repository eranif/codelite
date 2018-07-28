//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clPrintout.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLPRINTOUT_H
#define CLPRINTOUT_H

#include <wx/string.h>
#include <wx/gdicmn.h>
#include <wx/prntbase.h>

class clEditor;
class clPrintout : public wxPrintout
{
    clEditor* m_edit;
    wxArrayInt m_pageEnds;
    wxRect m_pageRect;
    wxRect m_printRect;
    int m_minPage;
    int m_maxPage;
    
protected:
    bool PrintScaling(wxDC* dc);

public:
    //! constructor
    clPrintout(clEditor* edit, const wxString& title = wxT(""));

    //! event handlers
    bool OnPrintPage(int page);
    bool OnBeginDocument(int startPage, int endPage);

    //! print functions
    bool HasPage(int page);
    void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo);
};

#endif // CLPRINTOUT_H
