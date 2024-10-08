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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "free_text_dialog.h"

#include "globals.h"

#include <wx/wx.h>
#include <wx/wxprec.h>

///////////////////////////////////////////////////////////////////////////

FreeTextDialog::FreeTextDialog(wxWindow* parent, const wxString& value, int id, const wxString& title, wxPoint pos,
                               wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer23;
    bSizer23 = new wxBoxSizer(wxVERTICAL);

    m_text = new clThemedSTC(this, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    m_text->SetWrapMode(wxSTC_WRAP_WORD);

    bSizer23->Add(m_text, 1, wxALL | wxEXPAND, 5);
    m_text->CallAfter(&clThemedSTC::SetFocus);

    m_staticline9 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizer23->Add(m_staticline9, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* bSizer24;
    bSizer24 = new wxBoxSizer(wxHORIZONTAL);

    m_buttonOK = new wxButton(this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer24->Add(m_buttonOK, 0, wxALL, 5);

    m_buttonCancel = new wxButton(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    bSizer24->Add(m_buttonCancel, 0, wxALL, 5);

    bSizer23->Add(bSizer24, 0, wxALIGN_RIGHT, 5);

    this->SetSizer(bSizer23);
    this->Layout();
    m_text->SetFocus();
    ::clSetDialogBestSizeAndPosition(this);
}
