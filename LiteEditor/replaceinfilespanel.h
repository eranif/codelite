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

#include <wx/combobox.h>
#include <wx/gauge.h>

class clThemedComboBox;
class ReplaceInFilesPanel : public FindResultsTab
{
protected:
    wxComboBox* m_replaceWith;
    wxGauge* m_progress;
    wxArrayString m_filesModified;

protected:
    /// Save the changes made to the file `sci` and mark its matches (`fileMatches`) as done or failed.
    /// `sci` is deleted if it is a temporary editor
    void DoSaveResults(wxStyledTextCtrl* sci, const std::vector<FindResultsNode*>& fileMatches);
    wxStyledTextCtrl* DoGetEditor(const wxString& fileName);

    /*
     * @brief get replacement text (regular expression backrefs applied)
     */
    wxString DoGetReplaceWith(const SearchResult& res) const;

    // Event handlers
    void OnSearchStart(wxCommandEvent& e) override;
    void OnSearchMatch(wxCommandEvent& e) override;
    void OnSearchEnded(wxCommandEvent& e) override;
    void OnHoldOpenUpdateUI(wxUpdateUIEvent& e) override;
    void ApplyTheme() override;

    virtual void OnMarkAll(wxCommandEvent& e);
    virtual void OnUnmarkAll(wxCommandEvent& e);
    virtual void OnReplace(wxCommandEvent& e);

    virtual void OnMarkAllUI(wxUpdateUIEvent& e);
    virtual void OnUnmarkAllUI(wxUpdateUIEvent& e);
    virtual void OnReplaceUI(wxUpdateUIEvent& e);
    virtual void OnReplaceWithComboUI(wxUpdateUIEvent& e);

public:
    ReplaceInFilesPanel(wxWindow* parent, wxWindowID id, const wxString& name);
    ~ReplaceInFilesPanel() override = default;
};

#endif // __replaceinfilespanel__
