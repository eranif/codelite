//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : findresultstab.cpp
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
#include "findresultstab.h"

#include "ColoursAndFontsManager.h"
#include "bitmap_loader.h"
#include "clStrings.h"
#include "clToolBarButtonBase.h"
#include "cl_aui_tool_stickness.h"
#include "cl_command_event.h"
#include "cl_config.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "manager.h"
#include "optionsconfig.h"
#include "pluginmanager.h"
#include "search_thread.h"

#include <algorithm>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

FindResultsTab::FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name, bool checkable)
    : wxPanel(parent, id)
    , m_searchInProgress(false)
    , m_name(name)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_vSizer = new wxBoxSizer(wxVERTICAL);
    GetSizer()->Add(m_vSizer, 1, wxEXPAND);

    // Create the toolbar
    m_tb = new wxAuiToolBar(this);
    auto images = clGetManager()->GetStdIcons();
    m_tb->AddTool(XRCID("hold_pane_open"),
                  _("Keep open"),
                  images->LoadBitmap("ToolPin"),
                  _("Don't close this pane when an editor gets focus"),
                  wxITEM_CHECK);
    m_tb->AddTool(XRCID("clear_all_output"), _("Clear All"), images->LoadBitmap("clear"), _("Clear All"));
    m_tb->AddTool(XRCID("collapse_all"), _("Fold All Results"), images->LoadBitmap("fold"), _("Fold All Results"));
    m_tb->AddTool(XRCID("repeat_output"), _("Repeat"), images->LoadBitmap("debugger_restart"), _("Repeat"));
    m_tb->AddTool(
        XRCID("stop_search"), _("Stop current search"), images->LoadBitmap("stop"), _("Stop current search"));
    m_tb->AddTool(XRCID("recent_searches"),
                  _("Show Recent Searches"),
                  images->LoadBitmap("history"),
                  _("Show Recent Searches"),
                  wxITEM_NORMAL);
    m_tb->SetToolDropDown(XRCID("recent_searches"), true);
    m_tb->Realize();

    // A line that describes the search (and, when it ends, its summary)
    m_statusLine = new FindResultsStatusLine(this);

    m_view = new FindResultsView(this, checkable);

    m_vSizer->Add(m_tb, 0, wxEXPAND);
    m_vSizer->Add(m_view, 1, wxEXPAND);
    m_vSizer->Add(m_statusLine, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP | wxBOTTOM, 3);
    m_vSizer->Layout();

    BindSearchEvents(this);
    m_view->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindResultsTab::OnItemActivated, this);
    m_view->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindResultsTab::OnSelectionChanged, this);
    wxTheApp->Connect(XRCID("find_in_files"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(FindResultsTab::OnFindInFiles),
                      nullptr,
                      this);
    m_tb->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &FindResultsTab::OnRecentSearches, this, XRCID("recent_searches"));
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnRecentSearchesUI, this, XRCID("recent_searches"));

    // The toolbar sends its events (wxEVT_TOOL) to us. Handle the menu events as well (shortcuts)
    auto bindAction = [this](int id, void (FindResultsTab::*handler)(wxCommandEvent&)) {
        Bind(wxEVT_TOOL, handler, this, id);
        Bind(wxEVT_MENU, handler, this, id);
    };
    bindAction(XRCID("hold_pane_open"), &FindResultsTab::OnToggleHoldOpen);
    bindAction(XRCID("clear_all_output"), &FindResultsTab::OnClearAll);
    bindAction(XRCID("collapse_all"), &FindResultsTab::OnCollapseAll);
    bindAction(XRCID("repeat_output"), &FindResultsTab::OnRepeatOutput);
    bindAction(XRCID("stop_search"), &FindResultsTab::OnStopSearch);
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnHoldOpenUpdateUI, this, XRCID("hold_pane_open"));
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnClearAllUI, this, XRCID("clear_all_output"));
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnCollapseAllUI, this, XRCID("collapse_all"));
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnRepeatOutputUI, this, XRCID("repeat_output"));
    Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnStopSearchUI, this, XRCID("stop_search"));

    // Copy / select all
    wxTheApp->Bind(wxEVT_MENU, &FindResultsTab::OnEdit, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_MENU, &FindResultsTab::OnEdit, this, wxID_SELECTALL);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnEditUI, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnEditUI, this, wxID_SELECTALL);

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &FindResultsTab::OnThemeChanged, this);
    // Use the same eventhandler for editor config changes too e.g. show/hide whitespace
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &FindResultsTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);

    FindResultsTab::ApplyTheme();
}

FindResultsTab::~FindResultsTab()
{
    UnbindSearchEvents(this);
    wxTheApp->Disconnect(XRCID("find_in_files"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(FindResultsTab::OnFindInFiles),
                         nullptr,
                         this);
    wxTheApp->Unbind(wxEVT_MENU, &FindResultsTab::OnEdit, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_MENU, &FindResultsTab::OnEdit, this, wxID_SELECTALL);
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &FindResultsTab::OnEditUI, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &FindResultsTab::OnEditUI, this, wxID_SELECTALL);
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &FindResultsTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &FindResultsTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);
}

void FindResultsTab::ApplyTheme()
{
    // The results use the colours of the current lexer theme
    m_view->ApplyTheme();

    // The status line uses the system colours (not the lexer ones), with the font of the results
    SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    m_statusLine->ApplyTheme(m_view->GetFont());
    Refresh();
}

void FindResultsTab::Clear()
{
    m_view->GetResultsModel()->Clear();
    m_currentMatch = nullptr;
    m_searchTitle.clear();
    m_statusLine->Clear();
}

void FindResultsTab::FoldResults()
{
    if (EditorConfigST::Get()->GetOptions()->GetDontAutoFoldResults()) {
        m_view->ExpandAllFiles();
    } else {
        // Collapse the results, but show the matches of the first file
        m_view->ExpandFirstFileOnly();
    }
}

void FindResultsTab::OnFindInFiles(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if (m_searchInProgress) {
        ::wxMessageBox(_("Another search is currently running, try again later"),
                       wxT("CodeLite"),
                       wxICON_WARNING | wxOK | wxOK_DEFAULT);
        return;
    }
    clGetManager()->OpenFindInFileForPaths({});
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
    m_searchInProgress = true;
    Clear();
    SearchData* data = (SearchData*)e.GetClientData();
    if (data) {
        m_searchData = *data;
        m_searchTitle = data->GetFindString();
        m_statusLine->ShowSearching(m_searchData);
    }
    wxDELETE(data);

    // Show this tab in the output pane (without taking the focus)
    ManagerST::Get()->ShowOutputPane(m_name, true, false);

    // Make sure that the Output view & the "Replace" tab
    // are visible

    clCommandEvent event(wxEVT_SHOW_OUTPUT_TAB);
    event.SetSelected(true).SetString(FIND_IN_FILES_WIN);
    EventNotifier::Get()->AddPendingEvent(event);
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
    SearchResultList* res = (SearchResultList*)e.GetClientData();
    if (!res) {
        return;
    }

    wxWindowUpdateLocker locker{m_view};
    m_view->GetResultsModel()->AddResults(*res);
    wxDELETE(res);
}

void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
{
    m_searchInProgress = false;
    SearchSummary* summary = (SearchSummary*)e.GetClientData();
    if (!summary)
        return;

    m_statusLine->ShowSummary(*summary, m_searchData);
    FoldResults();

    wxDELETE(summary);
    SaveSearchData();

    // We need to tell all editors that there's been a (new) search
    // This lets them clear any already-saved line-changes,
    // which a new save will have taken into account
    auto editors = clMainFrame::Get()->GetMainBook()->GetAllEditors();
    for (size_t n = 0; n < editors.size(); ++n) {
        clEditor* editor = dynamic_cast<clEditor*>(*(editors.begin() + n));
        if (editor) {
            editor->OnFindInFiles();
        }
    }
}

void FindResultsTab::OnSearchCancel(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_statusLine->ShowCancelled();
}

void FindResultsTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SearchThreadST::Get()->StopSearch();
    Clear();
}

void FindResultsTab::OnClearAllUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_searchInProgress && !m_view->GetResultsModel()->IsEmpty());
}

void FindResultsTab::OnRepeatOutput(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SearchData* searchData = GetSearchData();
    searchData->UseNewTab(false);
    SearchThreadST::Get()->PerformSearch(*searchData);
}

void FindResultsTab::OnRepeatOutputUI(wxUpdateUIEvent& e)
{
    e.Enable(!m_searchInProgress && !m_searchTitle.empty());
}

void FindResultsTab::OnCollapseAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    m_view->ToggleFolding();
}

void FindResultsTab::OnCollapseAllUI(wxUpdateUIEvent& e) { e.Enable(!m_view->GetResultsModel()->IsEmpty()); }

void FindResultsTab::OnSelectionChanged(wxDataViewEvent& e)
{
    // Selecting a match does the same as activating it. Ignore the selection that we make ourselves
    // (see OpenRelativeMatch), and a selection of several items
    if (m_selectingMatch || m_view->GetSelectedItemsCount() != 1) {
        return;
    }

    const FindResultsNode* node = FindResultsModel::ToNode(e.GetItem());
    if (node != nullptr && node->IsMatch()) {
        OpenMatch(node);
    }
}

void FindResultsTab::OnItemActivated(wxDataViewEvent& e)
{
    const FindResultsNode* node = FindResultsModel::ToNode(e.GetItem());
    if (node == nullptr || !node->IsMatch()) {
        // A file: let the control expand / collapse it
        e.Skip();
        return;
    }
    OpenMatch(node);
}

SearchData* FindResultsTab::GetSearchData() { return &m_searchData; }

void FindResultsTab::NextMatch() { OpenRelativeMatch(1); }

void FindResultsTab::PrevMatch() { OpenRelativeMatch(-1); }

void FindResultsTab::OpenRelativeMatch(int direction)
{
    const std::vector<FindResultsNode*> matches = m_view->GetResultsModel()->GetMatches();

    // The index of the current match: the match that was opened last, or the selected one
    int current = wxNOT_FOUND;
    auto indexOf = [&matches](const FindResultsNode* node) {
        auto iter = std::find(matches.begin(), matches.end(), node);
        return iter == matches.end() ? wxNOT_FOUND : static_cast<int>(std::distance(matches.begin(), iter));
    };
    if (m_currentMatch != nullptr) {
        current = indexOf(m_currentMatch);
    }
    if (current == wxNOT_FOUND) {
        const FindResultsNode* selected = FindResultsModel::ToNode(m_view->GetSelection());
        if (selected != nullptr && selected->IsMatch()) {
            current = indexOf(selected);
        }
    }

    // Without a current match, start from the first (or the last) match
    int next = current + direction;
    if (current == wxNOT_FOUND) {
        next = direction > 0 ? 0 : static_cast<int>(matches.size()) - 1;
    }
    if (next < 0 || next >= static_cast<int>(matches.size())) {
        // We are at the end (or at the top) of the search results list, add a status message
        clMainFrame::Get()->GetStatusBar()->SetMessage(direction > 0
                                                           ? _("Reached the end of the 'Find In Files' results")
                                                           : _("Reached the start of the 'Find In Files' results"));
        return;
    }

    const FindResultsNode* node = matches[next];
    m_selectingMatch = true;
    m_view->SelectNode(node);
    m_selectingMatch = false;
    OpenMatch(node);
}

void FindResultsTab::OpenMatch(const FindResultsNode* node)
{
    CHECK_PTR_RET(node);
    m_currentMatch = node;
    DoOpenSearchResult(node->result);
}

void FindResultsTab::DoOpenSearchResult(const SearchResult& result)
{
    if (!result.GetFileName().IsEmpty()) {
        // let plugins handle this first
        clFindInFilesEvent open_event(wxEVT_FINDINFILES_OPEN_MATCH);

        // prepare a single match entry and fire it
        clFindInFilesEvent::Match match;
        clFindInFilesEvent::Location loc;
        match.file = result.GetFileName();
        loc.column_start = result.GetColumn();
        loc.column_end = result.GetColumn() + result.GetLen();
        loc.line = result.GetLineNumber();
        match.locations.push_back(loc);
        open_event.GetMatches().push_back(match);

        if (EventNotifier::Get()->ProcessEvent(open_event)) {
            return;
        }

        clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(result.GetFileName());
        if (editor && result.GetLen() >= 0) {
            // Update the destination position if there have been subsequent changes in the editor
            int position = editor->PositionFromLine(result.GetLineNumber() - 1) + result.GetColumn();
            const std::vector<int> changes = editor->GetChanges();
            unsigned int changesTotal = changes.size();
            int changePosition = 0;
            int changeLength = 0;
            int resultLength = result.GetLen();
            bool removed = false;
            for (unsigned int i = 0; i < changesTotal; i += 2) {
                changePosition = changes.at(i);
                changeLength = changes.at(i + 1);
                if ((changeLength < 0) && (changePosition - changeLength > position) &&
                    (changePosition < position + resultLength)) {
                    // It looks like the data corresponding to this search result has been deleted
                    // While it's possible that it's been cut, then (later in the changes) re-pasted
                    // so that the result still matches, it's more likely to have been replaced by different text
                    // We can't easily tell, so assume the worst and label the result invalid
                    removed = true;
                    // Explain the failure
                    clMainFrame::Get()->GetStatusBar()->SetMessage(_("Search result is no longer valid"));
                    break;
                } else if (changePosition <= position) {
                    position += changeLength;
                }
            }
            if (!removed) {
                // use the "Async" way of loading the file and then selecting the match
                // this ensures that the results are visible and centered in screen
                int lineNumber = editor->LineFromPos(position);
                auto callback = [=](IEditor* p_editor) {
                    p_editor->GetCtrl()->ClearSelections();
                    p_editor->GetCtrl()->SetSelection(position, position + resultLength);
                    p_editor->CenterLinePreserveSelection(lineNumber);
                };
                clGetManager()->OpenFileAndAsyncExecute(editor->GetFileName().GetFullPath(), std::move(callback));
            }
        }
    }
}

void FindResultsTab::OnStopSearch(wxCommandEvent& e)
{
    // stop the search thread
    wxUnusedVar(e);
    // stop the search thread
    SearchThreadST::Get()->StopSearch();

    // and in case the search functionality is done by a plugin, fire an event
    clFindInFilesEvent stop_event{wxEVT_FINDINFILES_STOP_SEARCH};
    EventNotifier::Get()->AddPendingEvent(stop_event);
}

void FindResultsTab::OnStopSearchUI(wxUpdateUIEvent& e) { e.Enable(m_searchInProgress); }

void FindResultsTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if (clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

    if (EditorConfigST::Get()->GetOptions()->GetHideOutputPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfSearch());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void FindResultsTab::OnToggleHoldOpen(wxCommandEvent& e)
{
    int sel = wxNOT_FOUND;
    Notebook* book = clMainFrame::Get()->GetOutputPane()->GetNotebook();
    if (book && (sel = book->GetSelection()) != wxNOT_FOUND) {
        EditorConfigST::Get()->SetPaneStickiness(book->GetPageText(sel), e.IsChecked());
    }
}

bool FindResultsTab::IsFocused() const
{
    // The control has an internal window that gets the focus
    wxWindow* win = wxWindow::FindFocus();
    return win != nullptr && (win == m_view || win->GetParent() == m_view);
}

void FindResultsTab::OnEdit(wxCommandEvent& e)
{
    if (!IsFocused()) {
        e.Skip();
        return;
    }

    switch (e.GetId()) {
    case wxID_COPY: {
        const wxString text = m_view->GetSelectedText();
        if (!text.empty()) {
            ::CopyToClipboard(text);
        }
    } break;
    case wxID_SELECTALL:
        m_view->SelectAll();
        break;
    default:
        break;
    }
}

void FindResultsTab::OnEditUI(wxUpdateUIEvent& e)
{
    if (!IsFocused()) {
        return;
    }

    switch (e.GetId()) {
    case wxID_COPY:
        e.Enable(m_view->GetSelectedItemsCount() > 0);
        break;
    case wxID_SELECTALL:
        e.Enable(true);
        break;
    default:
        break;
    }
}

void FindResultsTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    ApplyTheme();
}

void FindResultsTab::OnRecentSearches(wxAuiToolBarEvent& e)
{
    // Show the menu
    wxMenu menu;
    const int firstID = 8000;
    int counter = 0;
    std::map<int, History> entries;
    for (const auto& p : m_history) {
        menu.Prepend(firstID + counter, p.first, "", wxITEM_CHECK)->Check(m_searchTitle == p.first);
        entries.insert(std::make_pair(firstID + counter, p.second));
        ++counter;
    }

    menu.AppendSeparator();
    int clearHistory = ::wxNewId();
    menu.Append(clearHistory, _("Clear History"));

    // line up our menu with the button
    clAuiToolStickness stickness{m_tb, e.GetId()};
    const wxRect rect = m_tb->GetToolRect(e.GetId());
    const wxPoint pt = ScreenToClient(m_tb->ClientToScreen(rect.GetBottomLeft()));
    int sel = GetPopupMenuSelectionFromUser(menu, pt);
    if (sel == wxID_NONE) {
        return;
    }
    if (sel == clearHistory) {
        m_history.Clear();

    } else if (entries.count(sel)) {
        const History& h = entries.find(sel)->second;
        LoadSearch(h);
    }
}

void FindResultsTab::SaveSearchData()
{
    History entry;
    entry.searchData = m_searchData;
    entry.title = m_searchTitle;
    for (const FindResultsNode* match : m_view->GetResultsModel()->GetMatches()) {
        entry.results.push_back(match->result);
    }

    // search for an entry with the same title
    if (m_history.Contains(entry.title)) {
        m_history.Remove(entry.title);
    }
    m_history.PushBack(entry.title, entry);
}

void FindResultsTab::LoadSearch(const History& h)
{
    m_searchData = h.searchData;
    m_searchTitle = h.title;
    m_currentMatch = nullptr;

    wxWindowUpdateLocker locker{m_view};
    m_view->GetResultsModel()->Clear();
    m_view->GetResultsModel()->AddResults(h.results);
    FoldResults();
    m_statusLine->ShowHistory(m_searchData, h.results.size());
}

void FindResultsTab::OnRecentSearchesUI(wxUpdateUIEvent& e) { e.Enable(!m_history.IsEmpty() && !m_searchInProgress); }

void FindResultsTab::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void FindResultsTab::UnbindSearchEvents(wxEvtHandler* binder)
{
    if (!m_searchEventsConnected)
        return;
    m_searchEventsConnected = false;
    binder->Unbind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &FindResultsTab::OnSearchStart, this);
    binder->Unbind(wxEVT_SEARCH_THREAD_MATCHFOUND, &FindResultsTab::OnSearchMatch, this);
    binder->Unbind(wxEVT_SEARCH_THREAD_SEARCHEND, &FindResultsTab::OnSearchEnded, this);
    binder->Unbind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &FindResultsTab::OnSearchCancel, this);
}

void FindResultsTab::BindSearchEvents(wxEvtHandler* binder)
{
    binder->Bind(wxEVT_SEARCH_THREAD_SEARCHSTARTED, &FindResultsTab::OnSearchStart, this);
    binder->Bind(wxEVT_SEARCH_THREAD_MATCHFOUND, &FindResultsTab::OnSearchMatch, this);
    binder->Bind(wxEVT_SEARCH_THREAD_SEARCHEND, &FindResultsTab::OnSearchEnded, this);
    binder->Bind(wxEVT_SEARCH_THREAD_SEARCHCANCELED, &FindResultsTab::OnSearchCancel, this);
    m_searchEventsConnected = true;
}

/////////////////////////////////////////////////////////////////////////////////

std::vector<int> EditorDeltasHolder::GetChanges()
{
    // There may have been net +ve or -ve position changes (i.e. undos) subsequent to a last save
    // and some of these may have then been overwritten by different ones. So we need to add both the originals and
    // current
    // The lengths of the originals must be negated (since they're based from the end, not the beginning).
    // Even if m_changes and m_changesForCurrentMatches have the same sizes, it may mean that nothing has changed, or
    // none since the last save,
    // but it may also mean that there have been n undos, followed by n different alterations. So we have to treat all
    // array sizes the same
    std::vector<int> changes;
    for (int index = m_changesForCurrentMatches.size() - 2; index >= 0; index -= 2) {
        changes.push_back(m_changesForCurrentMatches.at(index));      // position
        changes.push_back(-m_changesForCurrentMatches.at(index + 1)); // length
    }
    changes.insert(changes.end(), m_changes.begin(), m_changes.end());
    return changes;
}
