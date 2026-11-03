//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : findusagetab.h
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

#ifndef FINDUSAGETAB_H
#define FINDUSAGETAB_H

#include "LSP/LSPEvent.h"
#include "LSP/basic_types.h"
#include "clWorkspaceEvent.hpp"

#include <memory>
#include <vector>
#include <wx/aui/auibar.h>
#include <wx/dataview.h>
#include <wx/panel.h>

class FindUsageRenderer;
class FindUsageTextRenderer;
struct FindUsageEntry;
struct FindUsageFile;
class wxSearchCtrl;
class wxSplitterWindow;

/// The results of "Find References": the files on the left, the matches of the selected file on the right, and a filter
/// above them
class FindUsageTab : public wxPanel
{
    std::vector<LSP::Location> m_locations;
    std::vector<std::unique_ptr<FindUsageEntry>> m_entries; // all the matches
    std::vector<std::unique_ptr<FindUsageFile>> m_files; // the files, in the order of the view. They point to m_entries
    wxAuiToolBar* m_toolbar = nullptr;
    bool m_hideBuildFolders = true; // hide the results from the build folders (.build-release, ...)
    wxSearchCtrl* m_filter = nullptr;
    wxSplitterWindow* m_splitter = nullptr;
    wxDataViewListCtrl* m_dvFiles = nullptr;         // the files that have a match that passes the filter
    wxDataViewListCtrl* m_dvMatches = nullptr;       // the matches of the selected file: line | match
    FindUsageRenderer* m_renderer = nullptr;         // owned by the "Match" column of m_dvMatches
    FindUsageTextRenderer* m_fileRenderer = nullptr; // owned by the "File" column of m_dvFiles
    FindUsageTextRenderer* m_lineRenderer = nullptr; // owned by the "Line" column of m_dvMatches

private:
    void DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches);
    void InitialiseView(const std::vector<LSP::Location>& locations);
    /// Show the files that have a match that passes the filter, and the matches of the selected file
    void ApplyFilter();
    /// Show the matches of `file` that pass the filter
    void ShowFileMatches(const FindUsageFile* file);
    const FindUsageFile* GetSelectedFile() const;
    void UpdateStyle();
    /// (Re)create the buttons. Called again when the bitmaps change, to load the icons of the new theme
    void BuildToolbar();

public:
    FindUsageTab(wxWindow* parent);
    virtual ~FindUsageTab();

protected:
    void Clear();
    void OnClearAllUI(wxUpdateUIEvent& e);
    void OnClearAll(wxCommandEvent& e);
    void OnFilter(wxCommandEvent& event);
    void OnFileSelected(wxDataViewEvent& event);
    void OnThemeChanged(clCommandEvent& e);
    void OnBitmapsUpdated(clCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);
    void OnReferences(LSPEvent& event);
    void OnReferencesInProgress(LSPEvent& event);
    void OnItemActivated(wxDataViewEvent& event);
};

#endif // FINDUSAGETAB_H
