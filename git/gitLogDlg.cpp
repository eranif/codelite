//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : gitLogDlg.cpp
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

#include "gitLogDlg.h"
#include "windowattrmanager.h"

#ifndef __WXMSW__
#include "icons/icon_git.xpm"
#endif

GitLogDlg::GitLogDlg(wxWindow* parent, const wxString title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    SetIcon(wxICON(icon_git));
    m_editor = new wxTextCtrl(
        this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxStdDialogButtonSizer* sizer_3 = CreateStdDialogButtonSizer(wxOK);
    wxStaticBoxSizer* sizer_1 = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Log output"));
    sizer_1->Add(m_editor, 1, wxALL | wxEXPAND, 5);
    sizer->Add(sizer_1, 1, wxALL | wxEXPAND, 5);
    sizer->Add(sizer_3, 0, wxALL | wxEXPAND, 5);
    SetSizer(sizer);
    sizer->Fit(this);
    Layout();

    SetSize(1024, 560);
    SetName("GitLogDlg");
    WindowAttrManager::Load(this);
}

GitLogDlg::~GitLogDlg() {}

void GitLogDlg::SetLog(const wxString& log)
{
    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    wxTextAttr atr = m_editor->GetDefaultStyle();
    atr.SetFont(font);
    m_editor->SetDefaultStyle(atr);
    m_editor->AppendText(log);
}
