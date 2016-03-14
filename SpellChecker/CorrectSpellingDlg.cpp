//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CorrectSpellingDlg.cpp
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

/////////////////////////////////////////////////////////////////////////////
// Name:        correctspellingdlg.cpp
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: CorrectSpellingDlg.cpp 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#include "CorrectSpellingDlg.h"
#include "IHunSpell.h"
// ------------------------------------------------------------
CorrectSpellingDlg::CorrectSpellingDlg(wxWindow* parent)
    : CorrectSpellingDlg_base(parent)
{
    m_misspelled = wxT("");
    m_pHs = NULL;
    this->Connect(wxEVT_MOVE, wxMoveEventHandler(CorrectSpellingDlg::OnMove));
    m_currentPosition.x = -1;
    m_currentPosition.y = -1;
    GetSizer()->Fit(this);
}
// ------------------------------------------------------------
CorrectSpellingDlg::~CorrectSpellingDlg()
{
    this->Disconnect(wxEVT_MOVE, wxMoveEventHandler(CorrectSpellingDlg::OnMove));
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnInitDialog(wxInitDialogEvent& event)
{
    event.Skip();
    m_pMisspelling->SetValue(m_misspelled);

    if(m_currentPosition.x != -1) Move(m_currentPosition);
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnSuggestionSelected(wxCommandEvent& event)
{
    m_pMisspelling->SetValue(m_pSuggestions->GetString(event.GetInt()));
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnChangeClick(wxCommandEvent& event)
{
    m_misspelled = m_pMisspelling->GetValue();
    EndModal(SC_CHANGE);
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnIgnoreClick(wxCommandEvent& event) { EndModal(SC_IGNORE); }
// ------------------------------------------------------------
void CorrectSpellingDlg::OnDblClickSuggestions(wxCommandEvent& event)
{
    m_pMisspelling->SetValue(m_pSuggestions->GetString(event.GetInt()));
    m_misspelled = m_pMisspelling->GetValue();
    EndModal(SC_CHANGE);
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnAddClick(wxCommandEvent& event) { EndModal(SC_ADD); }
// ------------------------------------------------------------
void CorrectSpellingDlg::OnSuggestClick(wxCommandEvent& event)
{
    wxUnusedVar(event);

    if(m_pHs) {
        m_pSuggestions->Clear();
        wxArrayString suggests = m_pHs->GetSuggestions(m_pMisspelling->GetValue());

        for(wxUint32 i = 0; i < suggests.GetCount(); i++) m_pSuggestions->Append(suggests[i]);
    }
}
// ------------------------------------------------------------
void CorrectSpellingDlg::SetSuggestions(wxArrayString suggests)
{
    m_pSuggestions->Clear();

    for(wxUint32 i = 0; i < suggests.GetCount(); i++) m_pSuggestions->Append(suggests[i]);
}
// ------------------------------------------------------------
void CorrectSpellingDlg::OnMove(wxMoveEvent& event) { m_currentPosition = GetPosition(); }
// ------------------------------------------------------------
