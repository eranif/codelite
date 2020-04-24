//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : edit_workspace_conf_dlg.h
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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __edit_workspace_conf_dlg__
#define __edit_workspace_conf_dlg__

#include <wx/wx.h>

#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class EditWorkspaceConfDlg
///////////////////////////////////////////////////////////////////////////////
class EditWorkspaceConfDlg : public wxDialog
{
private:
    void CustomInit();
    void FillList();
    void DoRename(const wxString& selItem);

protected:
    wxListBox* m_wspConfList;
    wxButton* m_buttonRename;
    wxButton* m_buttonDelete;
    wxButton* m_buttonClose;

protected:
    // event handlers
    void OnDelete(wxCommandEvent& event);
    void OnRename(wxCommandEvent& event);
    void OnListBoxDClick(wxCommandEvent& event);

public:
    EditWorkspaceConfDlg(wxWindow* parent, int id = wxID_ANY, wxString title = _("Edit Workspace Configuration"),
                         wxPoint pos = wxDefaultPosition, wxSize size = wxSize(362, 261),
                         int style = wxDEFAULT_DIALOG_STYLE);
};

#endif //__edit_workspace_conf_dlg__
