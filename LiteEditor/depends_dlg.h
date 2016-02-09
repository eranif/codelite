//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : depends_dlg.h
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

#ifndef __depends_dlg__
#define __depends_dlg__

#include <wx/wx.h>

#include <wx/choicebk.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesDlg
///////////////////////////////////////////////////////////////////////////////
class DependenciesDlg : public wxDialog
{
private:
protected:
    wxChoicebook* m_book;
    wxStaticLine* m_staticline1;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;
    wxString m_projectName;

    void Init();
    void DoSelectProject();
    
    virtual void OnButtonOK(wxCommandEvent& event);
    virtual void OnButtonCancel(wxCommandEvent& event);

public:
    DependenciesDlg(wxWindow* parent,
        const wxString& projectName,
        int id = wxID_ANY,
        wxString title = _("Build Order"),
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxSize(700, 450),
        int style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~DependenciesDlg();
};

#endif //__depends_dlg__
