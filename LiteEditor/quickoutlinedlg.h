//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickoutlinedlg.h
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

#ifndef __quickoutlinedlg__
#define __quickoutlinedlg__

#include <wx/wx.h>

#include <wx/statline.h>
#include <wx/treectrl.h>
#include "wx/timer.h"

class CppSymbolTree;

///////////////////////////////////////////////////////////////////////////////
/// Class QuickOutlineDlg
///////////////////////////////////////////////////////////////////////////////
class QuickOutlineDlg : public wxDialog
{
    wxString m_fileName;

protected:
    wxStaticLine* m_staticline1;
    CppSymbolTree* m_treeOutline;

protected:
    void OnCharHook(wxKeyEvent& e);
    void OnTextEntered(wxCommandEvent& e);
    void OnItemSelected(wxCommandEvent& e);
    void OnKeyDown(wxKeyEvent& e);
    void DoParseActiveBuffer();
    
public:
    QuickOutlineDlg(wxWindow* parent,
                    const wxString& fileName,
                    int id = wxID_ANY,
                    wxString title = wxEmptyString,
                    wxPoint pos = wxDefaultPosition,
                    wxSize size = wxSize(371, 386),
                    int style = wxDEFAULT_DIALOG_STYLE);
    virtual ~QuickOutlineDlg();
};

#endif //__quickoutlinedlg__
