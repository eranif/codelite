//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findresultstab.h
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
#ifndef __findresultstab__
#define __findresultstab__

#include "FindResultsView.hpp"
#include "Notebook.h"
#include "clToolBar.h"
#include "clWorkspaceEvent.hpp"
#include "findinfilesdlg.h"
#include "outputtabwindow.h"
#include "search_thread.h"
#include "wx_ordered_map.h"

#include <map>
#include <unordered_map>
#include <vector>
#include <wx/aui/auibar.h>
#include <wx/debug.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>

/// The "Find In Files" results: a tree. The files are the top level items, and the matches are their children
class FindResultsTab : public wxPanel
{
protected:
    SearchData m_searchData;
    wxString m_searchTitle;
    bool m_searchInProgress;
    bool m_searchEventsConnected = false;

    struct History {
        wxString title;
        SearchData searchData;
        SearchResultList results;
        using Map_t = wxOrderedMap<wxString, History>;
    };

    History::Map_t m_history;

protected:
    wxString m_name;
    wxAuiToolBar* m_tb = nullptr;
    wxPanel* m_statusPanel = nullptr;  // draws a border around the status line
    wxStaticText* m_status = nullptr; // describes the search (or its summary)
    FindResultsView* m_view = nullptr;
    wxBoxSizer* m_vSizer = nullptr;
    // The match that was opened last. Used by NextMatch() / PrevMatch(). It is only compared, never accessed
    const FindResultsNode* m_currentMatch = nullptr;
    // True while we select a match in the view ourselves: this selection must not open the match again
    bool m_selectingMatch = false;

    void UnbindSearchEvents(wxEvtHandler* binder);
    void BindSearchEvents(wxEvtHandler* binder);

    /// Set the status line. The tooltip defaults to the text
    void SetStatus(const wxString& text, const wxString& tooltip = wxEmptyString);
    /// Remove all the results
    void Clear();
    void SaveSearchData();
    void LoadSearch(const History& h);
    /// Fold the files according to the user settings
    void FoldResults();
    virtual void OnFindInFiles(wxCommandEvent& e);
    virtual void OnRecentSearches(wxAuiToolBarEvent& e);
    virtual void OnSearchStart(wxCommandEvent& e);
    virtual void OnSearchMatch(wxCommandEvent& e);
    virtual void OnSearchEnded(wxCommandEvent& e);
    virtual void OnSearchCancel(wxCommandEvent& e);
    virtual void OnClearAll(wxCommandEvent& e);
    virtual void OnRepeatOutput(wxCommandEvent& e);
    virtual void OnCollapseAll(wxCommandEvent& e);
    virtual void OnToggleHoldOpen(wxCommandEvent& e);

    virtual void OnClearAllUI(wxUpdateUIEvent& e);
    virtual void OnRecentSearchesUI(wxUpdateUIEvent& e);
    virtual void OnRepeatOutputUI(wxUpdateUIEvent& e);
    virtual void OnCollapseAllUI(wxUpdateUIEvent& e);

    virtual void OnStopSearch(wxCommandEvent& e);
    virtual void OnStopSearchUI(wxUpdateUIEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    void OnItemActivated(wxDataViewEvent& e);
    void OnSelectionChanged(wxDataViewEvent& e);
    void OnEdit(wxCommandEvent& e);
    void OnEditUI(wxUpdateUIEvent& e);
    bool IsFocused() const;
    SearchData* GetSearchData();
    /// Open the match in an editor
    void OpenMatch(const FindResultsNode* node);
    void DoOpenSearchResult(const SearchResult& result);
    /// Open the next (`direction` is 1) or the previous (-1) match
    void OpenRelativeMatch(int direction);
    /// Use the colours and font of the output pane
    virtual void ApplyTheme();
    void OnThemeChanged(wxCommandEvent& e);
    void OnWorkspaceClosed(clWorkspaceEvent& event);

public:
    /// If `checkable` is true, each match has a check box (used by the "Replace In Files" tab)
    FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name, bool checkable = false);
    ~FindResultsTab() override;

    const wxString& GetCaption() const { return m_name; }
    void NextMatch();
    void PrevMatch();
};

class EditorDeltasHolder
{
    // This is all conceptually complex, so I'm being verbose for my own benefit
    // m_changes contains the current state: any position deltas since the file was loaded/reverted
    // m_changesAtLastSave is a snapshot of m_changes when the file was (last) saved
    // m_changesForCurrentMatches is a snapshot of m_changes at the last FileInFiles, so is the baseline for current
    // matches
    // (Any or all may be empty)
    // If the file is saved, that makes no difference to any existing FiF matches, so the above situation continues.
    // However we cache m_changes in m_changesAtLastSave
    // If another FindInFiles call is made, we no longer care about the m_changesForCurrentMatches contents as the
    // corresponding matches will have been overwritten.
    // So we replace m_changesForCurrentMatches with m_changesAtLastSave, which is the baseline for the new matches
    // Note that, unless the file is saved at some point after an initial FiF call, both will be empty
    // And, unless there's been a 'save' since the last FiF call, the contents of m_changesForCurrentMatches and
    // m_changesAtLastSave will be the same

    // When there's a 'GoTo next/previous FindInFiles' call, any relevant position changes need to be used. There are 4
    // possibilities:
    // 		1)	If there are no changes, FiF matches should 'just work'
    // 		2)	The common non-trivial situation is for there to be +ve position changes subsequent to the
    // file's
    // saved
    // status at the last FindInFiles call
    // 			However, see below...
    // 		3)	Occasionally there will have been -ve position changes (i.e. undos), or undos followed by
    // different
    // alterations.
    //			If there hasn't been a second FiF call, that won't matter.
    //		4)  If there *has* been a second FiF call, followed by more alterations, it *will* matter; especially if
    // there
    // have been undos, then different alterations.
    //			In that case we need to use both the original changes and the replacement ones.
    // As there's no easy way to tell the difference between 2) 3) and 4) (and the cost is nil for 1) anyway) treat all
    // cases as though they may be 4) instances.
    // That means combining m_changesForCurrentMatches (reversed and with lengths negated) and m_changes. See
    // GetChanges()

public:
    EditorDeltasHolder() = default;
    ~EditorDeltasHolder() { Clear(); }

    void Clear()
    {
        m_changes.clear();
        m_changesAtLastSave.clear();
        m_changesForCurrentMatches.clear();
    }
    void Push(int position, int length)
    {
        m_changes.push_back(position);
        m_changes.push_back(length);
    }

    void Pop()
    {
        if (m_changes.size() > 1) {
            m_changes.pop_back();
            m_changes.pop_back();
        }
    }

    void OnFileSaved() { m_changesAtLastSave = m_changes; }
    void OnFileInFiles() { m_changesForCurrentMatches = m_changesAtLastSave; }
    std::vector<int> GetChanges();

protected:
    std::vector<int> m_changes;
    std::vector<int> m_changesAtLastSave;
    std::vector<int> m_changesForCurrentMatches;
};

#endif // __findresultstab__
