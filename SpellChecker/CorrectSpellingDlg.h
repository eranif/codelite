//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : CorrectSpellingDlg.h
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
// Name:        correctspellingdlg.h
// Purpose:
// Author:      Frank Lichtner
// Modified by:
// Created:     02/02/14
// SVN-ID:      $Id: CorrectSpellingDlg.h 35 2014-02-22 18:18:49Z Frank $
// Copyright:   2014 Frank Lichtner
// License:
/////////////////////////////////////////////////////////////////////////////
#ifndef __CorrectSpellingDlg__
#define __CorrectSpellingDlg__
// ------------------------------------------------------------
#include "wxcrafter.h"
// ------------------------------------------------------------
#define SC_CHANGE 20
#define SC_IGNORE 21
#define SC_ADD 22
// ------------------------------------------------------------
/** Implementing CorrectSpellingDlg_base */
class IHunSpell;
class CorrectSpellingDlg : public CorrectSpellingDlg_base
{
protected:
    // Handlers for CorrectSpellingDlg_base events.
    void OnInitDialog(wxInitDialogEvent& event);
    void OnSuggestionSelected(wxCommandEvent& event);
    void OnChangeClick(wxCommandEvent& event);
    void OnIgnoreClick(wxCommandEvent& event);
    void OnDblClickSuggestions(wxCommandEvent& event);
    void OnAddClick(wxCommandEvent& event);
    void OnSuggestClick(wxCommandEvent& event);
    void OnMove(wxMoveEvent& event);

    wxString m_misspelled;
    IHunSpell* m_pHs;
    wxPoint m_currentPosition;

public:
    /** Constructor */
    CorrectSpellingDlg(wxWindow* parent);
    virtual ~CorrectSpellingDlg();

    void SetPHs(IHunSpell* pHs) { this->m_pHs = pHs; }
    wxString GetMisspelled() const { return m_misspelled; }
    void SetMisspelled(wxString value) { this->m_misspelled = value; }
    void SetSuggestions(wxArrayString suggests);
};

#endif // __CorrectSpellingDlg__
