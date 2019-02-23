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
#include "ColoursAndFontsManager.h"
#include "attribute_style.h"
#include "bitmap_loader.h"
#include "clStrings.h"
#include "clToolBarButtonBase.h"
#include "cl_aui_tool_stickness.h"
#include "cl_config.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "ctags_manager.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "findresultstab.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "manager.h"
#include "optionsconfig.h"
#include "pluginmanager.h"
#include "search_thread.h"
#include "theme_handler.h"
#include <algorithm>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

BEGIN_EVENT_TABLE(FindResultsTab, OutputTabWindow)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, FindResultsTab::OnSearchStart)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, FindResultsTab::OnSearchMatch)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, FindResultsTab::OnSearchEnded)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, FindResultsTab::OnSearchCancel)
EVT_UPDATE_UI(XRCID("hold_pane_open"), FindResultsTab::OnHoldOpenUpdateUI)
END_EVENT_TABLE()

FindResultsTab::FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name)
    : OutputTabWindow(parent, id, name)
    , m_searchInProgress(false)
{
    m_sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindResultsTab::OnStyleNeeded), NULL, this);

    BitmapLoader& loader = *(PluginManager::Get()->GetStdIcons());

    wxTheApp->Connect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
    m_tb->Bind(wxEVT_TOOL_DROPDOWN, &FindResultsTab::OnRecentSearches, this, XRCID("recent_searches"));
    m_tb->Bind(wxEVT_UPDATE_UI, &FindResultsTab::OnRecentSearchesUI, this, XRCID("recent_searches"));

    m_tb->AddTool(XRCID("stop_search"), _("Stop current search"), loader.LoadBitmap("stop"), _("Stop current search"));
    m_tb->AddTool(XRCID("recent_searches"), _("Show Recent Searches"), loader.LoadBitmap("history"),
                  _("Show Recent Searches"), wxITEM_DROPDOWN);

    Connect(XRCID("stop_search"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FindResultsTab::OnStopSearch),
            NULL, this);
    Connect(XRCID("stop_search"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindResultsTab::OnStopSearchUI), NULL, this);
    m_tb->Realize();

    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnThemeChanged), NULL,
                                  this);

    // Use the same eventhandler for editor config changes too e.g. show/hide whitespace
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &FindResultsTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);
}

FindResultsTab::~FindResultsTab()
{
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnThemeChanged), NULL,
                                  this);
    wxTheApp->Disconnect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindResultsTab::OnWorkspaceClosed, this);
}

void FindResultsTab::SetStyles(wxStyledTextCtrl* sci) { m_styler->SetStyles(sci); }

void FindResultsTab::AppendText(const wxString& line)
{
    m_sci->SetIndicatorCurrent(1);
    OutputTabWindow::AppendText(line);
}

void FindResultsTab::Clear()
{
    m_matchInfo.clear();
    m_indicators.clear();
    m_searchTitle.clear();
    OutputTabWindow::Clear();
    m_styler->Reset();
}

void FindResultsTab::OnFindInFiles(wxCommandEvent& e)
{
    if(m_searchInProgress) {
        ::wxMessageBox(_("The search thread is currently busy"), _("CodeLite"), wxICON_INFORMATION | wxOK);
        return;
    }

    // Fire the wxEVT_CMD_FIND_IN_FILES_SHOWING showing event
    clFindInFilesEvent eventFifShowing(wxEVT_FINDINFILES_DLG_SHOWING);
    EventNotifier::Get()->ProcessEvent(eventFifShowing);

    // Display the Find In Files dialog
    FindReplaceData frd;
    frd.SetName("FindInFilesData");
    clConfig::Get().ReadItem(&frd);
    
    // Allocate the 'Find In Files' in an inner block
    // We do this because the 'FindReplaceData' will be updated upon the destruction of the dialog
    {
        bool sendDismissEvent = true;
        const wxString& mask = eventFifShowing.GetFileMask();

        // plugins provided paths
        const wxString& paths = eventFifShowing.GetPaths();

        // transient paths take precedence over the normal paths. However, they are not persistent
        // Usually these paths are given when the a tree view like control has focus and user selected folders in it
        const wxString& transientPaths = eventFifShowing.GetTransientPaths();

        wxString fifPaths = paths;
        if(!transientPaths.IsEmpty()) {
            fifPaths = transientPaths;
            sendDismissEvent = false;
        }

        // Prepare the fif dialog
        FindInFilesDialog dlg(EventNotifier::Get()->TopFrame(), frd);
        if(!fifPaths.IsEmpty()) { dlg.SetSearchPaths(fifPaths, !transientPaths.IsEmpty()); }
        if(!mask.IsEmpty()) { dlg.SetFileMask(mask); }

        // Show it
        if((dlg.ShowDialog() == wxID_OK) && sendDismissEvent) {
            // Notify about the dialog dismissal
            clFindInFilesEvent eventDismiss(wxEVT_FINDINFILES_DLG_DISMISSED);
            eventDismiss.SetFileMask(frd.GetSelectedMask());
            eventDismiss.SetPaths(frd.GetSearchPaths());
            EventNotifier::Get()->ProcessEvent(eventDismiss);
        }
        
    }
    // And we alway store the global find-in-files data (it keeps the 'find-what', 'replace with' fields, etc...)
    clConfig::Get().WriteItem(&frd);
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
    m_searchInProgress = true;
    Clear();
    SetStyles(m_sci);
    SearchData* data = (SearchData*)e.GetClientData();
    if(data) {
        m_searchData = *data;
        m_searchTitle = data->GetFindString();

        wxString message;
        message << _("====== Searching for: '") << data->GetFindString() << _("'; Match case: ")
                << (data->IsMatchCase() ? _("true") : _("false")) << _(" ; Match whole word: ")
                << (data->IsMatchWholeWord() ? _("true") : _("false")) << _(" ; Regular expression: ")
                << (data->IsRegularExpression() ? _("true") : _("false")) << wxT(" ======\n");
        AppendText(message);
    }
    wxDELETE(data);

    // Make sure that the Output view & the "Replace" tab
    // are visible

    clCommandEvent event(wxEVT_SHOW_OUTPUT_TAB);
    event.SetSelected(true).SetString(FIND_IN_FILES_WIN);
    EventNotifier::Get()->AddPendingEvent(event);
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
    SearchResultList* res = (SearchResultList*)e.GetClientData();
    if(!res) return;

    SearchResultList::iterator iter = res->begin();
    for(; iter != res->end(); ++iter) {
        if(m_matchInfo.empty() || m_matchInfo.rbegin()->second.GetFileName() != iter->GetFileName()) {
            if(!m_matchInfo.empty()) { AppendText("\n"); }
            wxFileName fn(iter->GetFileName());
            fn.MakeRelativeTo();
            AppendText(fn.GetFullPath() + wxT("\n"));
        }

        int lineno = m_sci->GetLineCount() - 1;
        m_matchInfo.insert(std::make_pair(lineno, *iter));
        wxString text = iter->GetPattern();
        // int delta = -text.Length();
        // text.Trim(false);
        // delta += text.Length();
        // text.Trim();

        wxString linenum = wxString::Format(wxT(" %5u: "), iter->GetLineNumber());
        SearchData* d = GetSearchData();
        // Print the scope name
        if(d->GetDisplayScope()) {
            TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->GetFileName(), iter->GetLineNumber());
            wxString scopeName(wxT("global"));
            if(tag) { scopeName = tag->GetPath(); }

            linenum << wxT("[ ") << scopeName << wxT(" ] ");
            iter->SetScope(scopeName);
        }

        AppendText(linenum + text + wxT("\n"));
        int indicatorStartPos = m_sci->PositionFromLine(lineno) + iter->GetColumn() + linenum.Length();
        int indicatorLen = iter->GetLen();
        m_indicators.push_back(indicatorStartPos);
        m_sci->IndicatorFillRange(indicatorStartPos, indicatorLen);
    }
    wxDELETE(res);
}

void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
{
    m_searchInProgress = false;
    SearchSummary* summary = (SearchSummary*)e.GetClientData();
    if(!summary) return;

    // did the page closed before the search ended?
    AppendText(summary->GetMessage() + wxT("\n"));

    if(m_tb->FindById(XRCID("scroll_on_output")) && m_tb->FindById(XRCID("scroll_on_output"))->IsChecked()) {
        m_sci->GotoLine(0);
    }

    if(!EditorConfigST::Get()->GetOptions()->GetDontAutoFoldResults()) {
        OutputTabWindow::OnCollapseAll(e);
        // Uncollapse the first file's matches
        int maxLine = m_sci->GetLineCount();
        for(int line = 0; line < maxLine; line++) {
            int foldLevel = (m_sci->GetFoldLevel(line) & wxSTC_FOLDLEVELNUMBERMASK) - wxSTC_FOLDLEVELBASE;
            if(foldLevel == 2 && !m_sci->GetFoldExpanded(line)) {
                m_sci->ToggleFold(line);
                break;
            }
        }
    }

    wxDELETE(summary);
    SaveSearchData();

    // We need to tell all editors that there's been a (new) search
    // This lets them clear any already-saved line-changes,
    // which a new save will have taken into account
    clEditor::Vec_t editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t n = 0; n < editors.size(); ++n) {
        clEditor* editor = dynamic_cast<clEditor*>(*(editors.begin() + n));
        if(editor) { editor->OnFindInFiles(); }
    }
}

void FindResultsTab::OnSearchCancel(wxCommandEvent& e) { AppendText(_("====== Search cancelled by user ======\n")); }

void FindResultsTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SearchThreadST::Get()->StopSearch();
    Clear();
}

void FindResultsTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(!m_searchInProgress && !m_sci->IsEmpty()); }

void FindResultsTab::OnRepeatOutput(wxCommandEvent& e)
{
    wxUnusedVar(e);
    SearchData* searchData = GetSearchData();
    searchData->UseNewTab(false);
    SearchThreadST::Get()->PerformSearch(*searchData);
}

void FindResultsTab::OnRepeatOutputUI(wxUpdateUIEvent& e) { e.Enable(m_sci->GetLength() > 0); }

void FindResultsTab::OnMouseDClick(wxStyledTextEvent& e)
{
    int clickedLine = wxNOT_FOUND;
    m_styler->HitTest(m_sci, e, clickedLine);

    // Did we clicked on a togglable line?
    int toggleLine = m_styler->TestToggle(m_sci, e);
    if(toggleLine != wxNOT_FOUND) {
        m_sci->ToggleFold(toggleLine);

    } else {
        MatchInfo_t::const_iterator m = m_matchInfo.find(clickedLine);
        if(m != m_matchInfo.end()) { DoOpenSearchResult(m->second, m_sci, m->first); }
    }
}

SearchData* FindResultsTab::GetSearchData() { return &m_searchData; }

void FindResultsTab::NextMatch()
{
    // locate the last match
    int firstLine = m_sci->MarkerNext(0, 255);
    if(firstLine == wxNOT_FOUND) { firstLine = 0; }

    // We found the last marker
    for(int i = firstLine + 1; i < m_sci->GetLineCount(); i++) {

        // Find the next match
        MatchInfo_t::const_iterator iter = m_matchInfo.find(i);
        if(iter != m_matchInfo.end()) {
            SearchResult sr = iter->second;

            // open the new searchresult in the editor
            DoOpenSearchResult(sr, m_sci, i);
            return;
        }
    }

    // if we are here, it means we are the end of the search results list, add a status message
    clMainFrame::Get()->GetStatusBar()->SetMessage(_("Reached the end of the 'Find In Files' results"));
}

void FindResultsTab::PrevMatch()
{
    // locate the last match
    int firstLine = m_sci->MarkerPrevious(m_sci->GetLineCount() - 1, 255);
    if(firstLine == wxNOT_FOUND) { firstLine = m_sci->GetLineCount(); }

    // We found the last marker
    for(int i = firstLine - 1; i >= 0; i--) {

        // Find the next match
        MatchInfo_t::const_iterator iter = m_matchInfo.find(i);
        if(iter != m_matchInfo.end()) {
            SearchResult sr = iter->second;

            // open the new searchresult in the editor
            DoOpenSearchResult(sr, m_sci, i);
            return;
        }
    }
    // if we are here, it means we are the top of the search results list, add a status message
    clMainFrame::Get()->GetStatusBar()->SetMessage(_("Reached the start of the 'Find In Files' results"));
}

void FindResultsTab::DoOpenSearchResult(const SearchResult& result, wxStyledTextCtrl* sci, int markerLine)
{
    if(!result.GetFileName().IsEmpty()) {
        clEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(result.GetFileName());
        if(editor && result.GetLen() >= 0) {
            // Update the destination position if there have been subsequent changes in the editor
            int position = result.GetPosition();
            std::vector<int> changes;
            editor->GetChanges(changes);
            unsigned int changesTotal = changes.size();
            int changePosition = 0;
            int changeLength = 0;
            int resultLength = result.GetLen();
            bool removed = false;
            for(unsigned int i = 0; i < changesTotal; i += 2) {
                changePosition = changes.at(i);
                changeLength = changes.at(i + 1);
                if((changeLength < 0) && (changePosition - changeLength > position) &&
                   (changePosition < position + resultLength)) {
                    // It looks like the data corresponding to this search result has been deleted
                    // While it's possible that it's been cut, then (later in the changes) re-pasted
                    // so that the result still matches, it's more likely to have been replaced by different text
                    // We can't easily tell, so assume the worst and label the result invalid
                    removed = true;
                    // Explain the failure
                    clMainFrame::Get()->GetStatusBar()->SetMessage(_("Search result is no longer valid"));
                    break;
                } else if(changePosition <= position) {
                    position += changeLength;
                }
            }
            if(!removed) {
                editor->SetEnsureCaretIsVisible(
                    position + resultLength, true,
                    true); // The 3rd parameter sets a small delay, otherwise it fails for long folded files
                int lineNumber = editor->LineFromPos(position);
                if(lineNumber) { lineNumber--; }
                editor->SetLineVisible(lineNumber);
                editor->SetSelection(position, position + resultLength);

#ifdef __WXGTK__
                editor->ScrollToColumn(0);
#endif

                if(sci) {
                    // remove the previous marker and add the new one
                    sci->MarkerDeleteAll(7);
                    sci->MarkerAdd(markerLine, 7);
                    sci->EnsureVisible(markerLine);
                    sci->GotoLine(markerLine);
                }
            }
        }
    }
}

void FindResultsTab::OnStopSearch(wxCommandEvent& e)
{
    // stop the search thread
    SearchThreadST::Get()->StopSearch();
}

void FindResultsTab::OnStopSearchUI(wxUpdateUIEvent& e) { e.Enable(m_searchInProgress); }

void FindResultsTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) { return; }

    if(EditorConfigST::Get()->GetOptions()->GetHideOutpuPaneOnUserClick()) {
        e.Enable(true);
        e.Check(EditorConfigST::Get()->GetOptions()->GetHideOutputPaneNotIfSearch());

    } else {
        e.Enable(false);
        e.Check(false);
    }
}

void FindResultsTab::OnStyleNeeded(wxStyledTextEvent& e)
{
    wxStyledTextCtrl* ctrl = dynamic_cast<wxStyledTextCtrl*>(e.GetEventObject());
    if(!ctrl) return;
    StyleText(ctrl, e);
}

void FindResultsTab::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope)
{
    m_styler->StyleText(ctrl, e, hasSope);
}

void FindResultsTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    SetStyles(m_sci);
}

void FindResultsTab::OnRecentSearches(wxCommandEvent& e)
{
    // Show the menu
    wxMenu menu;
    const int firstID = 8000;
    int counter = 0;
    std::map<int, History> entries;
    std::for_each(m_history.Begin(), m_history.End(), [&](const std::pair<wxString, History>& p) {
        menu.Prepend(firstID + counter, p.first, "", wxITEM_CHECK)->Check(m_searchTitle == p.first);
        entries.insert(std::make_pair(firstID + counter, p.second));
        ++counter;
    });

    clToolBarButtonBase* button = m_tb->FindById(e.GetId());
    CHECK_PTR_RET(button);

    menu.AppendSeparator();
    int clearHistory = ::wxNewId();
    menu.Append(clearHistory, _("Clear History"));
    int sel = m_tb->GetMenuSelectionFromUser(XRCID("recent_searches"), &menu);
    if(sel == wxID_NONE) { return; }
    if(sel == clearHistory) {
        m_history.Clear();

    } else if(entries.count(sel)) {
        const History& h = entries.find(sel)->second;
        LoadSearch(h);
    }
}

void FindResultsTab::SaveSearchData()
{
    History entry;
    entry.text = m_sci->GetText();
    entry.searchData = m_searchData;
    entry.title = m_searchTitle;
    entry.matchInfo = m_matchInfo;

    // Save the indicators as well
    entry.indicators = m_indicators;

    // search for an entry with the same title
    if(m_history.Contains(entry.title)) { m_history.Remove(entry.title); }
    m_history.PushBack(entry.title, entry);
}

void FindResultsTab::LoadSearch(const History& h)
{
    m_searchData = h.searchData;
    m_matchInfo = h.matchInfo;
    m_searchTitle = h.title;
    m_sci->SetEditable(true);
    m_sci->ClearAll();
    m_sci->SetText(h.text);

    // restore the indicators
    std::for_each(h.indicators.begin(), h.indicators.end(),
                  [&](int pos) { m_sci->IndicatorFillRange(pos, h.title.length()); });
    m_sci->SetFirstVisibleLine(0);
    m_sci->SetEditable(false);
}

void FindResultsTab::OnRecentSearchesUI(wxUpdateUIEvent& e) { e.Enable(!m_history.IsEmpty() && !m_searchInProgress); }

void FindResultsTab::ResetStyler() { m_styler->Reset(); }

void FindResultsTab::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();
    Clear();
}

/////////////////////////////////////////////////////////////////////////////////

void EditorDeltasHolder::GetChanges(std::vector<int>& changes)
{
    // There may have been net +ve or -ve position changes (i.e. undos) subsequent to a last save
    // and some of these may have then been overwritten by different ones. So we need to add both the originals and
    // current
    // The lengths of the originals must be negated (since they're based from the end, not the beginning).
    // Even if m_changes and m_changesForCurrentMatches have the same sizes, it may mean that nothing has changed, or
    // none since the last save,
    // but it may also mean that there have been n undos, followed by n different alterations. So we have to treat all
    // array sizes the same
    changes.clear();
    for(int index = m_changesForCurrentMatches.size() - 2; index >= 0; index -= 2) {
        changes.push_back(m_changesForCurrentMatches.at(index));      // position
        changes.push_back(-m_changesForCurrentMatches.at(index + 1)); // length
    }
    changes.insert(changes.end(), m_changes.begin(), m_changes.end());
}
