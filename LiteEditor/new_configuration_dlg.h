//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : new_configuration_dlg.h
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

#ifndef __new_configuration_dlg__
#define __new_configuration_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class NewConfigurationDlg
///////////////////////////////////////////////////////////////////////////////
class NewConfigurationDlg : public wxDialog
{
private:
    wxString m_projectName;
    wxPanel* m_panel1;
    wxStaticText* m_staticText1;
    wxTextCtrl* m_textConfigurationName;
    wxStaticText* m_staticText2;
    wxChoice* m_choiceCopyConfigurations;
    wxStaticLine* m_staticline1;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

protected:
    void OnButtonOK(wxCommandEvent& event);

public:
    NewConfigurationDlg(wxWindow* parent, const wxString& projName, int id = wxID_ANY,
                        wxString title = _("New Configuration"), wxPoint pos = wxDefaultPosition,
                        wxSize size = wxSize(352, 199), int style = wxDEFAULT_DIALOG_STYLE);
};

#endif //__new_configuration_dlg__
