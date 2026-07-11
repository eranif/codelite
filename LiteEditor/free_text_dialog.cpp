//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : free_text_dialog.cpp
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

#include "free_text_dialog.h"

#include "globals.h"

///////////////////////////////////////////////////////////////////////////

FreeTextDialog::FreeTextDialog(
    wxWindow* parent, const wxString& value, int id, const wxString& title, wxPoint pos, wxSize size, int style)
    : FreeTextDialogBase(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    m_text->SetWrapMode(wxSTC_WRAP_WORD);
    m_text->SetValue(value);
    m_text->CallAfter(&clThemedSTC::SetFocus);

    this->Layout();
    m_text->SetFocus();
    ::clSetDialogBestSizeAndPosition(*this);
}
