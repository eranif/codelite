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

#include <vector>
#include <map>
#include <list>
#include <wx/stc/stc.h>
#include "wx/debug.h"

#include "Notebook.h"
#include "outputtabwindow.h"
#include "search_thread.h"
#include "findinfilesdlg.h"
#include "wx_ordered_map.h"
#include <wx/aui/auibar.h>

// Map between the line numbers and a search results
typedef std::map<int, SearchResult> MatchInfo_t;

class FindResultsTab : public OutputTabWindow
{
protected:
    SearchData m_searchData;
    wxString m_searchTitle;
    std::list<int> m_indicators;
    bool m_searchInProgress;

    struct History
    {
        wxString title;
        SearchData searchData;
        wxString text;
        MatchInfo_t matchInfo;
        std::list<int> indicators;
        typedef wxOrderedMap<wxString, History> Map_t;
    };

    History::Map_t m_history;

protected:
    MatchInfo_t m_matchInfo;

    void AppendText(const wxString& line);
    void Clear();
    void SaveSearchData();
    void LoadSearch(const History& h);
    virtual void OnFindInFiles(wxCommandEvent& e);
    virtual void OnRecentSearches(wxAuiToolBarEvent& e);
    virtual void OnSearchStart(wxCommandEvent& e);
    virtual void OnSearchMatch(wxCommandEvent& e);
    virtual void OnSearchEnded(wxCommandEvent& e);
    virtual void OnSearchCancel(wxCommandEvent& e);
    virtual void OnClearAll(wxCommandEvent& e);
    virtual void OnRepeatOutput(wxCommandEvent& e);

    virtual void OnClearAllUI(wxUpdateUIEvent& e);
    virtual void OnRecentSearchesUI(wxUpdateUIEvent& e);
    virtual void OnRepeatOutputUI(wxUpdateUIEvent& e);
    virtual void OnMouseDClick(wxStyledTextEvent& e);

    virtual void OnStopSearch(wxCommandEvent& e);
    virtual void OnStopSearchUI(wxUpdateUIEvent& e);
    virtual void OnHoldOpenUpdateUI(wxUpdateUIEvent& e);
    virtual void OnStyleNeeded(wxStyledTextEvent& e);
    SearchData* GetSearchData();
    void DoOpenSearchResult(const SearchResult& result, wxStyledTextCtrl* sci, int markerLine);
    void OnThemeChanged(wxCommandEvent& e);
    void OnWorkspaceClosed(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()

public:
    FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name);
    ~FindResultsTab();

    void SetStyles(wxStyledTextCtrl* sci);
    void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope = false);
    void ResetStyler();

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
    EditorDeltasHolder() {}
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
        if(m_changes.size() > 1) {
            m_changes.pop_back();
            m_changes.pop_back();
        }
    }

    void OnFileSaved() { m_changesAtLastSave = m_changes; }
    void OnFileInFiles() { m_changesForCurrentMatches = m_changesAtLastSave; }
    void GetChanges(std::vector<int>& changes);

protected:
    std::vector<int> m_changes;
    std::vector<int> m_changesAtLastSave;
    std::vector<int> m_changesForCurrentMatches;
};

#endif // __findresultstab__
