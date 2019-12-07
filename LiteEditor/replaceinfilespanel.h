//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : replaceinfilespanel.h
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
#ifndef __replaceinfilespanel__
#define __replaceinfilespanel__

#include "findresultstab.h"

class ReplaceInFilesPanel : public FindResultsTab
{
protected:
    wxComboBox* m_replaceWith;
    wxGauge* m_progress;
    wxStaticText* m_replaceWithText;
    wxArrayString m_filesModified;
    wxBitmap m_bmpChecked;
    wxBitmap m_bmpUnchecked;
    bool m_bmpsForDarkTheme = false;
    
protected:
    void DoSaveResults(wxStyledTextCtrl* sci, std::map<int, SearchResult>::iterator begin,
                       std::map<int, SearchResult>::iterator end);

    wxStyledTextCtrl* DoGetEditor(const wxString& fileName);

    // Event handlers
    virtual void OnSearchStart(wxCommandEvent& e);
    virtual void OnSearchMatch(wxCommandEvent& e);
    virtual void OnSearchEnded(wxCommandEvent& e);
    virtual void OnMarginClick(wxStyledTextEvent& e);

    virtual void OnMarkAll(wxCommandEvent& e);
    virtual void OnUnmarkAll(wxCommandEvent& e);
    virtual void OnReplace(wxCommandEvent& e);

    virtual void OnMarkAllUI(wxUpdateUIEvent& e);
    virtual void OnUnmarkAllUI(wxUpdateUIEvent& e);
    virtual void OnReplaceUI(wxUpdateUIEvent& e);
    virtual void OnReplaceWithComboUI(wxUpdateUIEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    virtual void OnMouseDClick(wxStyledTextEvent& e);

public:
    ReplaceInFilesPanel(wxWindow* parent, wxWindowID id, const wxString& name);
    virtual ~ReplaceInFilesPanel();
    
    virtual void SetStyles(wxStyledTextCtrl* sci);
};

#endif // __replaceinfilespanel__
