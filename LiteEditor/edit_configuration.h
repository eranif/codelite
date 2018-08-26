//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : edit_configuration.h
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

#ifndef __edit_configuration__
#define __edit_configuration__

#include <wx/wx.h>

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class EditConfigurationDialog
///////////////////////////////////////////////////////////////////////////////
class EditConfigurationDialog : public wxDialog
{
private:
protected:
    wxPanel* m_panel6;
    wxListBox* m_configurationsList;
    wxButton* m_buttonDelete;
    wxButton* m_buttonRename;
    wxStaticLine* m_staticline9;
    wxButton* m_buttonClose;
    wxString m_projectName;

    void OnItemDclick(wxCommandEvent& event);
    void OnButtonClose(wxCommandEvent& event);
    void OnButtonRename(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);

    void RenameConfiguration(const wxString& oldName, const wxString& newName);

public:
    EditConfigurationDialog(wxWindow* parent, const wxString& projectName, int id = wxID_ANY,
        wxString title = _("Edit Configurations"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(338, 199),
        int style = wxDEFAULT_DIALOG_STYLE);
};

#endif //__edit_configuration__
