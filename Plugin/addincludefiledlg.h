//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : addincludefiledlg.h
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
#ifndef __addincludefiledlg__
#define __addincludefiledlg__

#include "addincludefiledlgbase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK AddIncludeFileDlg : public AddIncludeFileDlgBase
{
    wxString m_fullpath;
    wxString m_text;
    int m_line;
    static wxArrayString m_includePath;
    wxString m_lineToAdd;
    bool m_isLineToAddProvided = false;

protected:
    void SetAndMarkLine();
    void OnTextUpdated(wxCommandEvent& e);
    void OnButtonDown(wxCommandEvent& event);
    void OnButtonUp(wxCommandEvent& event);
    void OnButtonOK(wxCommandEvent& e);
    void OnClearCachedPaths(wxCommandEvent& e);
    void UpdateLineToAdd();

    void OnPreviewKeyDown(wxKeyEvent& event);
    void OnIdle(wxIdleEvent& event);

public:
    /** Constructor */
    AddIncludeFileDlg(wxWindow* parent, const wxString& fullpath, const wxString& text, int lineNo);
    virtual ~AddIncludeFileDlg();
    const wxString& GetLineToAdd() const { return m_lineToAdd; }
    int GetLine() const { return m_line; }
};

#endif // __addincludefiledlg__
