//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : renamefilebasedlg.cpp
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

#include "renamefilebasedlg.h"

///////////////////////////////////////////////////////////////////////////

RenameFileBaseDlg::RenameFileBaseDlg(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
                                     const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* bSizer3;
    bSizer3 = new wxBoxSizer(wxHORIZONTAL);

    m_staticText8 = new wxStaticText(this, wxID_ANY, _("Replace With:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText8->Wrap(-1);
    bSizer3->Add(m_staticText8, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_textCtrlReplaceWith =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    bSizer3->Add(m_textCtrlReplaceWith, 1, wxALL | wxEXPAND, 5);

    mainSizer->Add(bSizer3, 0, wxEXPAND, 5);

    m_staticText1 = new wxStaticText(this, wxID_ANY, _("The following files will be updated:"), wxDefaultPosition,
                                     wxDefaultSize, 0);
    m_staticText1->Wrap(-1);
    mainSizer->Add(m_staticText1, 0, wxALL | wxEXPAND, 5);

    wxArrayString m_checkListMatchesChoices;
    m_checkListMatches =
        new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMatchesChoices, 0);
    mainSizer->Add(m_checkListMatches, 1, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* sbSizer1;
    sbSizer1 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer(0, 2, 0, 0);
    fgSizer1->AddGrowableCol(1);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    m_staticText3 = new wxStaticText(this, wxID_ANY, _("Included From:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText3->Wrap(-1);
    m_staticText3->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    fgSizer1->Add(m_staticText3, 0, wxALL, 5);

    m_staticTextIncludedInFile = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextIncludedInFile->Wrap(-1);
    fgSizer1->Add(m_staticTextIncludedInFile, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText5 = new wxStaticText(this, wxID_ANY, _("Line:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText5->Wrap(-1);
    m_staticText5->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    fgSizer1->Add(m_staticText5, 0, wxALL, 5);

    m_staticTextFoundInLine = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextFoundInLine->Wrap(-1);
    fgSizer1->Add(m_staticTextFoundInLine, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText9 = new wxStaticText(this, wxID_ANY, _("Pattern:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText9->Wrap(-1);
    m_staticText9->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

    fgSizer1->Add(m_staticText9, 0, wxALL, 5);

    m_staticTextPattern = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextPattern->Wrap(-1);
    fgSizer1->Add(m_staticTextPattern, 0, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

    sbSizer1->Add(fgSizer1, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(sbSizer1, 0, wxEXPAND | wxALL, 5);

    wxBoxSizer* buttonSizer;
    buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_buttonOk = new wxButton(this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0);
    m_buttonOk->SetDefault();
    buttonSizer->Add(m_buttonOk, 0, wxALL, 5);

    m_buttonCancel = new wxButton(this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    buttonSizer->Add(m_buttonCancel, 0, wxALL, 5);

    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    this->SetSizer(mainSizer);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    m_checkListMatches->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
                                wxCommandEventHandler(RenameFileBaseDlg::OnFileSelected), NULL, this);
    m_checkListMatches->Connect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
                                wxCommandEventHandler(RenameFileBaseDlg::OnFileToggeled), NULL, this);
}

RenameFileBaseDlg::~RenameFileBaseDlg()
{
    // Disconnect Events
    m_checkListMatches->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED,
                                   wxCommandEventHandler(RenameFileBaseDlg::OnFileSelected), NULL, this);
    m_checkListMatches->Disconnect(wxEVT_COMMAND_CHECKLISTBOX_TOGGLED,
                                   wxCommandEventHandler(RenameFileBaseDlg::OnFileToggeled), NULL, this);
}
