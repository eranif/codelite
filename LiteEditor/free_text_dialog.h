//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : free_text_dialog.h
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

#ifndef __free_text_dialog__
#define __free_text_dialog__

#include <wx/wx.h>

#include <wx/statline.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class FreeTextDialog
///////////////////////////////////////////////////////////////////////////////
class FreeTextDialog : public wxDialog
{
protected:
    wxTextCtrl* m_text;
    wxStaticLine* m_staticline9;
    wxButton* m_buttonOK;
    wxButton* m_buttonCancel;

public:
    FreeTextDialog(wxWindow* parent,
                   wxString value = wxEmptyString,
                   int id = wxID_ANY,
                   wxString title = _("Edit"),
                   wxPoint pos = wxDefaultPosition,
                   wxSize size = wxSize(481, 299),
                   int style = wxDEFAULT_DIALOG_STYLE);
    wxString GetValue() const { return m_text->GetValue(); }
};

#endif //__free_text_dialog__
