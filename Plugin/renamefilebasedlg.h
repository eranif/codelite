//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : renamefilebasedlg.h
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __renamefilebasedlg__
#define __renamefilebasedlg__

#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/colour.h>
#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class RenameFileBaseDlg
///////////////////////////////////////////////////////////////////////////////
class RenameFileBaseDlg : public wxDialog
{
private:
protected:
    wxStaticText* m_staticText8;
    wxTextCtrl* m_textCtrlReplaceWith;
    wxStaticText* m_staticText1;
    wxCheckListBox* m_checkListMatches;
    wxStaticText* m_staticText3;
    wxStaticText* m_staticTextIncludedInFile;
    wxStaticText* m_staticText5;
    wxStaticText* m_staticTextFoundInLine;
    wxStaticText* m_staticText9;
    wxStaticText* m_staticTextPattern;
    wxButton* m_buttonOk;
    wxButton* m_buttonCancel;

    // Virtual event handlers, overide them in your derived class
    virtual void OnFileSelected(wxCommandEvent& event) { event.Skip(); }
    virtual void OnFileToggeled(wxCommandEvent& event) { event.Skip(); }

public:
    RenameFileBaseDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Fix Include Statement"),
                      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(574, 437),
                      long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    ~RenameFileBaseDlg();
};

#endif //__renamefilebasedlg__
