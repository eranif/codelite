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

#pragma once

#include "clThemedSTC.hpp"

#include <wx/button.h>
#include <wx/statline.h>
#include <wx/wx.h>

///////////////////////////////////////////////////////////////////////////

class FreeTextDialog : public wxDialog
{
protected:
    clThemedSTC* m_text = nullptr;
    wxStaticLine* m_staticline9 = nullptr;
    wxButton* m_buttonOK = nullptr;
    wxButton* m_buttonCancel = nullptr;

public:
    FreeTextDialog(wxWindow* parent, const wxString& value = wxEmptyString, int id = wxID_ANY,
                   const wxString& title = _("Edit"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize(481, 299),
                   int style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxString GetValue() const { return m_text->GetValue(); }
};
