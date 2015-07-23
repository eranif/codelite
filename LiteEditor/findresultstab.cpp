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
#include <wx/xrc/xmlres.h>
#include "bitmap_loader.h"
#include <wx/wupdlock.h>
#include "drawingutils.h"
#include "event_notifier.h"
#include <wx/tokenzr.h>
#include "editor_config.h"
#include "globals.h"
#include "manager.h"
#include "frame.h"
#include "ctags_manager.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "pluginmanager.h"
#include "globals.h"
#include "findresultstab.h"
#include "search_thread.h"
#include "event_notifier.h"
#include "theme_handler.h"
#include "cl_config.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "attribute_style.h"

// Custom styles
#define LEX_FIF_DEFAULT 0
#define LEX_FIF_FILE 1
#define LEX_FIF_MATCH 2
#define LEX_FIF_LINE_NUMBER 3
#define LEX_FIF_HEADER 4
#define LEX_FIF_SCOPE 5
#define LEX_FIF_MATCH_COMMENT 6

class MySTC : public wxStyledTextCtrl
{
public:
    MySTC(wxWindow* parent)
        : wxStyledTextCtrl(parent)
    {
    }
    virtual ~MySTC()
    {
        if(HasCapture()) {
            ReleaseMouse();
        }
    }
};

BEGIN_EVENT_TABLE(FindResultsTab, OutputTabWindow)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, FindResultsTab::OnSearchStart)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, FindResultsTab::OnSearchMatch)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, FindResultsTab::OnSearchEnded)
EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, FindResultsTab::OnSearchCancel)

// Right click menu options
EVT_MENU(XRCID("close_fif_tab"), FindResultsTab::OnCloseTab)
EVT_MENU(XRCID("close_all_fif_tabs"), FindResultsTab::OnCloseAllTabs)
EVT_MENU(XRCID("close_other_fif_tabs"), FindResultsTab::OnCloseOtherTab)

EVT_UPDATE_UI(XRCID("close_fif_tab"), FindResultsTab::OnTabMenuUI)
EVT_UPDATE_UI(XRCID("close_all_fif_tabs"), FindResultsTab::OnTabMenuUI)
EVT_UPDATE_UI(XRCID("close_other_fif_tabs"), FindResultsTab::OnTabMenuUI)
EVT_UPDATE_UI(XRCID("hold_pane_open"), FindResultsTab::OnHoldOpenUpdateUI)
END_EVENT_TABLE()

FindResultsTab::FindResultsTab(wxWindow* parent, wxWindowID id, const wxString& name, bool useBook)
    : OutputTabWindow(parent, id, name)
    , m_searchInProgress(false)
    , m_book(NULL)
    , m_recv(NULL)
    , m_matchInfo(1)
{
    if(useBook) {

        // load the book style from the settings file
        long style = kNotebook_MouseMiddleClickClosesTab |      // Handle mouse middle button when clicked on a tab
                     kNotebook_MouseMiddleClickFireEvent |      // instead of closing the tab, fire an event
                     kNotebook_CloseButtonOnActiveTab |         // Show close button on the active tab
                     kNotebook_ShowFileListButton |             // show drop down list of all open tabs
                     kNotebook_CloseButtonOnActiveTabFireEvent; // When closing the 'x' button, fire an event

        m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
        m_book->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("find_in_files_right_click_menu")));

        m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &FindResultsTab::OnPageChanged, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CLOSED, &FindResultsTab::OnPageClosed, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CLOSING, &FindResultsTab::OnPageClosing, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &FindResultsTab::OnClosePage, this);

        // get rid of base class scintilla component
        wxSizer* sz = m_hSizer;
        sz->Detach(m_sci);
        m_sci->Destroy();
        m_sci = NULL;
#ifdef __WXMAC__
        sz->Insert(0, m_book, 1, wxALL | wxEXPAND);
#else
        sz->Add(m_book, 1, wxALL | wxEXPAND);
#endif
        sz->Layout();
    } else {
        // keep existing scintilla
        SetStyles(m_sci);
        m_sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindResultsTab::OnStyleNeeded), NULL, this);
    }

    BitmapLoader& loader = *(PluginManager::Get()->GetStdIcons());

    wxTheApp->Connect(XRCID("find_in_files"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(FindResultsTab::OnFindInFiles),
                      NULL,
                      this);
    m_tb->AddTool(XRCID("stop_search"),
                  _("Stop current search"),
                  loader.LoadBitmap(wxT("toolbars/16/build/stop")),
                  _("Stop current search"));
    Connect(XRCID("stop_search"),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(FindResultsTab::OnStopSearch),
            NULL,
            this);
    Connect(XRCID("stop_search"), wxEVT_UPDATE_UI, wxUpdateUIEventHandler(FindResultsTab::OnStopSearchUI), NULL, this);
    m_tb->Realize();

    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnThemeChanged), NULL, this);
}

FindResultsTab::~FindResultsTab()
{
    EventNotifier::Get()->Connect(
        wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(FindResultsTab::OnThemeChanged), NULL, this);
    wxTheApp->Disconnect(XRCID("find_in_files"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(FindResultsTab::OnFindInFiles),
                         NULL,
                         this);
}

MatchInfo& FindResultsTab::GetMatchInfo(size_t idx)
{
    ListMatchInfos::iterator itMatchInfo = m_matchInfo.begin();
    if(m_book) {
        for(size_t i = 0; i < idx; ++i) {
            ++itMatchInfo;
        }
    }
    return *itMatchInfo;
}

void FindResultsTab::SetStyles(wxStyledTextCtrl* sci)
{

    //#define LEX_FIF_DEFAULT 0
    //#define LEX_FIF_FILE 1
    //#define LEX_FIF_MATCH 2
    //#define LEX_FIF_LINE_NUMBER 3
    //#define LEX_FIF_HEADER 4
    //#define LEX_FIF_SCOPE 5
    //#define LEX_FIF_MATCH_COMMENT 6
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }

    const StyleProperty& defaultStyle = lexer->GetProperty(0);
    wxFont defaultFont = lexer->GetFontForSyle(0);
    
    for(size_t i = 0; i < wxSTC_STYLE_MAX; ++i) {
        sci->StyleSetForeground(i, defaultStyle.GetFgColour());
        sci->StyleSetBackground(i, defaultStyle.GetBgColour());
        sci->StyleSetFont(i, defaultFont);
    }

    StyleProperty::Map_t& props = lexer->GetLexerProperties();
    sci->StyleSetForeground(LEX_FIF_HEADER, props[11].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_HEADER, props[11].GetBgColour());
    
    // 33 is the style for line numbers
    sci->StyleSetForeground(LEX_FIF_LINE_NUMBER, props[33].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_LINE_NUMBER, props[33].GetBgColour());
    
    // 11 is the style number for "identifier"
    sci->StyleSetForeground(LEX_FIF_MATCH, props[11].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_MATCH, props[11].GetBgColour());
    
    // 16 is the stule for colouring classes
    sci->StyleSetForeground(LEX_FIF_SCOPE, props[16].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_SCOPE, props[16].GetBgColour());
    
    sci->StyleSetForeground(LEX_FIF_MATCH_COMMENT, props[wxSTC_C_COMMENTLINE].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_MATCH_COMMENT, props[wxSTC_C_COMMENTLINE].GetBgColour());

    sci->StyleSetForeground(LEX_FIF_FILE, props[wxSTC_C_WORD].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_FILE, props[wxSTC_C_WORD].GetBgColour());
    sci->StyleSetEOLFilled(LEX_FIF_FILE, true);

    sci->StyleSetForeground(LEX_FIF_DEFAULT, props[11].GetFgColour());
    sci->StyleSetBackground(LEX_FIF_DEFAULT, props[11].GetBgColour());

    sci->StyleSetHotSpot(LEX_FIF_MATCH, true);
    sci->StyleSetHotSpot(LEX_FIF_FILE, true);
    sci->StyleSetHotSpot(LEX_FIF_MATCH_COMMENT, true);

    sci->SetHotspotActiveForeground(true, lexer->IsDark() ? "WHITE" : "BLACK");
    sci->SetHotspotActiveUnderline(false);

    sci->MarkerDefine(7, wxSTC_MARK_ARROW);
    sci->MarkerSetBackground(7, lexer->IsDark() ? "YELLOW" : "BLACK");
    sci->MarkerSetForeground(7, lexer->IsDark() ? "YELLOW" : "BLACK");

    sci->IndicatorSetForeground(1, lexer->IsDark() ? "YELLOW" : "DARK GREY");
    sci->IndicatorSetStyle(1, wxSTC_INDIC_TEXTFORE);
    sci->IndicatorSetUnder(1, true);

    sci->SetMarginWidth(0, 0);
    sci->SetMarginWidth(1, 16);
    sci->SetMarginWidth(2, 0);
    sci->SetMarginWidth(3, 0);
    sci->SetMarginWidth(4, 0);
    sci->SetMarginSensitive(1, true);
    sci->HideSelection(true);
    sci->Refresh();
}

size_t FindResultsTab::GetPageCount() const
{
    if(m_book) {
        return m_book->GetPageCount();
    } else {
        return 1;
    }
}

void FindResultsTab::AppendText(const wxString& line)
{
    wxStyledTextCtrl* save = NULL;
    if(m_recv) {
        // so OutputTabWindow::AppendText() writes to the correct page
        save = m_sci;
        m_sci = m_recv;
    }
    m_sci->SetIndicatorCurrent(1);
    OutputTabWindow::AppendText(line);
    if(save) {
        m_sci = save;
    }
}

void FindResultsTab::Clear()
{
    MatchInfo& matchInfo = GetMatchInfo(m_book ? m_book->GetSelection() : 0);
    matchInfo.clear();
    OutputTabWindow::Clear();
}

void FindResultsTab::OnPageChanged(wxBookCtrlEvent& e)
{
    // this function can't be called unless m_book != NULL
    m_sci = dynamic_cast<wxStyledTextCtrl*>(m_book->GetCurrentPage());
    if(m_sci) {
        m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSTC_WRAP_WORD);
    }
}

void FindResultsTab::OnPageClosed(wxBookCtrlEvent& e)
{
    // this function can't be called unless m_book != NULL
    int sel = e.GetSelection();
    if(sel != wxNOT_FOUND) {
        ListMatchInfos::iterator itMatchInfo = m_matchInfo.begin();
        for(int i = 0; i < e.GetSelection(); ++i) {
            ++itMatchInfo;
        }
        m_matchInfo.erase(itMatchInfo);

    } else if(m_book->GetPageCount()) {
        m_matchInfo.clear();
    }

    // Create a page if there is no more
    if(m_book->GetPageCount() == 0) {
        m_sci = NULL;
    } else {
        m_sci = dynamic_cast<wxStyledTextCtrl*>(m_book->GetCurrentPage());
    }
}

void FindResultsTab::OnFindInFiles(wxCommandEvent& e)
{
    if(m_searchInProgress) {
        ::wxMessageBox(_("The search thread is currently busy"), _("CodeLite"), wxICON_INFORMATION | wxOK);
        return;
    }

    FindInFilesDialog dlg(EventNotifier::Get()->TopFrame(), "FindInFilesData");
    wxArrayString* paths = (wxArrayString*)e.GetClientData();
    if(paths) {
        dlg.SetSearchPaths(*paths);
        e.SetClientData(NULL);
        wxDELETE(paths);
    }
    dlg.ShowDialog();
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
    m_searchInProgress = true;
    SearchData* data = (SearchData*)e.GetClientData();
    wxString label = data ? data->GetFindString() : wxT("");

    if(e.GetInt() != 0 || m_sci == NULL) {
        if(m_book) {
            clWindowUpdateLocker locker(this);
            MySTC* sci = new MySTC(m_book);
            SetStyles(sci);
            sci->Connect(wxEVT_STC_STYLENEEDED, wxStyledTextEventHandler(FindResultsTab::OnStyleNeeded), NULL, this);
            m_book->AddPage(sci, label, true);
#ifdef __WXMAC__
            m_book->GetSizer()->Layout();
#endif
            size_t where = m_book->GetPageCount() - 1;

            // keep the search data used for this tab
            wxWindow* tab = m_book->GetPage(where);
            if(tab) {
                tab->SetClientData(data);
            }

            m_matchInfo.push_back(MatchInfo());
            m_sci = sci;
        }
    } else if(m_book) {
        // using current tab, update the tab title and the search data
        int where = m_book->GetPageIndex(m_sci);
        if(where != wxNOT_FOUND) {
            m_book->SetPageText(where, label);
            // delete the old search data
            wxWindow* tab = m_book->GetPage(where);
            SearchData* oldData = (SearchData*)tab->GetClientData();
            if(oldData) {
                delete oldData;
            }
            // set the new search data
            tab->SetClientData(data);
        }
    }

    // This is needed in >=wxGTK-2.9, otherwise the 'Search' pane doesn't fully expand
    SendSizeEvent(wxSEND_EVENT_POST);

    m_recv = m_sci;
    Clear();

    if(data) {
        m_searchData = *data;

        wxString message;
        message << _("====== Searching for: '") << data->GetFindString() << _("'; Match case: ")
                << (data->IsMatchCase() ? _("true") : _("false")) << _(" ; Match whole word: ")
                << (data->IsMatchWholeWord() ? _("true") : _("false")) << _(" ; Regular expression: ")
                << (data->IsRegularExpression() ? _("true") : _("false")) << wxT(" ======\n");
        AppendText(message);
    }
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
    SearchResultList* res = (SearchResultList*)e.GetClientData();
    if(!res) return;

    int m = m_book ? m_book->GetPageIndex(m_recv) : 0;
    if(m == wxNOT_FOUND) {
        wxDELETE(res);
        return;
    }

    MatchInfo& matchInfo = GetMatchInfo(m);
    for(SearchResultList::iterator iter = res->begin(); iter != res->end(); iter++) {
        if(matchInfo.empty() || matchInfo.rbegin()->second.GetFileName() != iter->GetFileName()) {
            if(!matchInfo.empty()) {
                AppendText("\n");
            }
            wxFileName fn(iter->GetFileName());
            fn.MakeRelativeTo();
            AppendText(fn.GetFullPath() + wxT("\n"));
        }

        int lineno = m_recv->GetLineCount() - 1;
        matchInfo.insert(std::make_pair(lineno, *iter));
        wxString text = iter->GetPattern();
        int delta = -text.Length();
        text.Trim(false);
        delta += text.Length();
        text.Trim();

        wxString linenum;
        if(iter->GetMatchState() == CppWordScanner::STATE_CPP_COMMENT ||
           iter->GetMatchState() == CppWordScanner::STATE_C_COMMENT)
            linenum = wxString::Format(wxT(" %5u //"), iter->GetLineNumber());
        else
            linenum = wxString::Format(wxT(" %5u "), iter->GetLineNumber());

        SearchData* d = GetSearchData(m_recv);
        // Print the scope name
        if(d->GetDisplayScope()) {
            TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->GetFileName(), iter->GetLineNumber());
            wxString scopeName(wxT("global"));
            if(tag) {
                scopeName = tag->GetPath();
            }

            linenum << wxT("[ ") << scopeName << wxT(" ] ");
            iter->SetScope(scopeName);
        }

        delta += linenum.Length();
        AppendText(linenum + text + wxT("\n"));
        m_recv->IndicatorFillRange(m_sci->PositionFromLine(lineno) + iter->GetColumn() + delta, iter->GetLen());
    }
    wxDELETE(res);
}

void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
{
    m_searchInProgress = false;
    SearchSummary* summary = (SearchSummary*)e.GetClientData();
    if(!summary) return;

    // did the page closed before the search ended?
    if(m_book && m_book->GetPageIndex(m_recv) != wxNOT_FOUND) {

        AppendText(summary->GetMessage() + wxT("\n"));
        m_recv = NULL;
        if(m_tb->GetToolToggled(XRCID("scroll_on_output"))) {
            m_sci->GotoLine(0);
        }

        if(!EditorConfigST::Get()->GetOptions()->GetDontAutoFoldResults()) {
            OutputTabWindow::OnCollapseAll(e);
            if(m_sci) {
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
        }
    } else if((m_recv == m_sci) && m_sci) {
        // Replace In Files...
        AppendText(summary->GetMessage() + wxT("\n"));
        if(m_tb->GetToolToggled(XRCID("scroll_on_output"))) {
            m_sci->GotoLine(0);
        }

        if(!EditorConfigST::Get()->GetOptions()->GetDontAutoFoldResults()) {
            OutputTabWindow::OnCollapseAll(e);
            if(m_sci) {
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
        }
    }
    delete summary;

    // We need to tell all editors that there's been a (new) search
    // This lets them clear any already-saved line-changes,
    // which a new save will have taken into account
    LEditor::Vec_t editors;
    clMainFrame::Get()->GetMainBook()->GetAllEditors(editors, MainBook::kGetAll_IncludeDetached);
    for(size_t n = 0; n < editors.size(); ++n) {
        LEditor* editor = dynamic_cast<LEditor*>(*(editors.begin() + n));
        if(editor) {
            editor->OnFindInFiles();
        }
    }
}

void FindResultsTab::OnSearchCancel(wxCommandEvent& e)
{
    m_searchInProgress = false;
    wxString* str = (wxString*)e.GetClientData();
    if(!str) return;

    // did the page closed before the search ended?
    if(m_book && m_book->GetPageIndex(m_recv) != wxNOT_FOUND) {
        AppendText(*str + wxT("\n"));
    }

    delete str;
    m_recv = NULL;
}

void FindResultsTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_recv != NULL) {
        SearchThreadST::Get()->StopSearch();
    }
    Clear();
}

void FindResultsTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(m_recv != NULL || (m_sci && m_sci->GetLength() > 0)); }

void FindResultsTab::OnRepeatOutput(wxCommandEvent& e)
{
    wxUnusedVar(e);

    if(m_book) {
        int sel = m_book->GetSelection();
        if(sel != wxNOT_FOUND) {
            // get the search data used to generate the output on the selected tab
            wxWindow* tab = m_book->GetPage(sel);
            if(tab) {
                SearchData* searchData = (SearchData*)tab->GetClientData();
                searchData->UseNewTab(false);
                SearchThreadST::Get()->PerformSearch(*searchData);
            }
        }
    }
}

void FindResultsTab::OnRepeatOutputUI(wxUpdateUIEvent& e)
{
    e.Enable(m_recv == NULL && m_sci && m_sci->GetLength() > 0);
}

void FindResultsTab::OnMouseDClick(wxStyledTextEvent& e)
{
    long pos = e.GetPosition();
    int line = m_sci->LineFromPosition(pos);
    int style = m_sci->GetStyleAt(pos);

    if(style == LEX_FIF_FILE || style == LEX_FIF_HEADER) {
        m_sci->ToggleFold(line);

    } else {
        int n = m_book ? m_book->GetSelection() : 0;
        const MatchInfo& matchInfo = GetMatchInfo(n);
        MatchInfo::const_iterator m = matchInfo.find(line);
        if(m != matchInfo.end()) {
            DoOpenSearchResult(m->second, m_sci, m->first);
        }
    }

    m_sci->SetSelection(wxNOT_FOUND, pos);
}

long FindResultsTab::GetBookStyle() { return 0; }

SearchData* FindResultsTab::GetSearchData(wxStyledTextCtrl* sci)
{
    if(m_book) {
        int i = m_book->GetPageIndex(sci);
        if(i != wxNOT_FOUND) {
            wxWindow* tab = m_book->GetPage(i);
            if(tab) {
                SearchData* data = (SearchData*)tab->GetClientData();
                if(data) {
                    return data;
                }
            }
        }
        return NULL;
    } else {
        // in case we dont have a notebook (e.g. 'Replace In Files')
        // we use the global saved search data
        return &m_searchData;
    }
}

void FindResultsTab::OnCloseAllTabs(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_book) {
        m_book->DeleteAllPages();
        m_sci = NULL;
    }
}

void FindResultsTab::OnCloseOtherTab(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_book) {
        int idx = m_book->GetSelection();
        if(idx != wxNOT_FOUND) {
            for(int i = 0; i < idx; i++) {
                m_book->DeletePage((size_t)0);
            }

            size_t number = m_book->GetPageCount();
            for(size_t i = number - 1; i > 0; i--) {
                m_book->DeletePage(i);
            }
        }
    }
}

void FindResultsTab::OnCloseTab(wxCommandEvent& e)
{
    wxUnusedVar(e);
    if(m_book) {
        int idx = m_book->GetSelection();
        if(idx != wxNOT_FOUND) {
            m_book->DeletePage(idx);
        }
    }
}

void FindResultsTab::OnTabMenuUI(wxUpdateUIEvent& e) { e.Enable(!m_searchInProgress); }

void FindResultsTab::NextMatch()
{
    // m_sci holds to the selected tab's scintilla editor
    if(m_sci) {
        const MatchInfo& matchInfo = GetMatchInfo(m_book ? m_book->GetSelection() : 0);

        // locate the last match
        int firstLine = m_sci->MarkerNext(0, 255);
        if(firstLine == wxNOT_FOUND) {
            firstLine = 0;
        }

        // We found the last marker
        for(int i = firstLine + 1; i < m_sci->GetLineCount(); i++) {

            // Find the next match
            MatchInfo::const_iterator iter = matchInfo.find(i);
            if(iter != matchInfo.end()) {
                SearchResult sr = iter->second;

                // open the new searchresult in the editor
                DoOpenSearchResult(sr, m_sci, i);
                return;
            }
        }

        // if we are here, it means we are the end of the search results list, add a status message
        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Reached the end of the 'Find In Files' results"));
    }
}

void FindResultsTab::PrevMatch()
{
    // m_sci holds to the selected tab's scintilla editor
    if(m_sci) {
        const MatchInfo& matchInfo = GetMatchInfo(m_book ? m_book->GetSelection() : 0);

        // locate the last match
        int firstLine = m_sci->MarkerPrevious(m_sci->GetLineCount() - 1, 255);
        if(firstLine == wxNOT_FOUND) {
            firstLine = m_sci->GetLineCount();
        }

        // We found the last marker
        for(int i = firstLine - 1; i >= 0; i--) {

            // Find the next match
            MatchInfo::const_iterator iter = matchInfo.find(i);
            if(iter != matchInfo.end()) {
                SearchResult sr = iter->second;

                // open the new searchresult in the editor
                DoOpenSearchResult(sr, m_sci, i);
                return;
            }
        }
        // if we are here, it means we are the top of the search results list, add a status message
        clMainFrame::Get()->GetStatusBar()->SetMessage(_("Reached the start of the 'Find In Files' results"));
    }
}

void FindResultsTab::DoOpenSearchResult(const SearchResult& result, wxStyledTextCtrl* sci, int markerLine)
{
    if(!result.GetFileName().IsEmpty()) {
        LEditor* editor = clMainFrame::Get()->GetMainBook()->OpenFile(result.GetFileName());
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
                    position,
                    true,
                    true); // The 3rd parameter sets a small delay, otherwise it fails for long folded files
                int lineNumber = editor->LineFromPos(position);
                if(lineNumber) {
                    lineNumber--;
                }
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

void FindResultsTab::OnClosePage(wxBookCtrlEvent& e)
{
    int where = e.GetSelection();
    if(where == wxNOT_FOUND) {
        return;
    }
    m_book->DeletePage((size_t)where);
}

void FindResultsTab::OnHoldOpenUpdateUI(wxUpdateUIEvent& e)
{
    int sel = clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetSelection();
    if(clMainFrame::Get()->GetOutputPane()->GetNotebook()->GetPage(sel) != this) {
        return;
    }

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

void FindResultsTab::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e)
{
    int startPos = ctrl->GetEndStyled();
    int endPos = e.GetPosition();
    wxString text = ctrl->GetTextRange(startPos, endPos);

    wxArrayString lines = ::wxStringTokenize(text, wxT("\r\n"), wxTOKEN_RET_DELIMS);
    ctrl->StartStyling(startPos, 0x1f); // text styling

    int bytes_left = 0;
    bool inMatchLine = false;
    int offset = 0;
    for(size_t i = 0; i < lines.GetCount(); ++i) {
        wxString curline = lines.Item(i);
        bytes_left = curline.length();
        offset = 0;

        if(curline.StartsWith("/")) {
            ctrl->SetStyling(curline.Length(), LEX_FIF_MATCH_COMMENT);
            bytes_left = 0;

        } else if(curline.StartsWith(wxT(" "))) {
            ctrl->SetStyling(6, LEX_FIF_LINE_NUMBER); // first 6 chars are the line number
            bytes_left -= 6;
            inMatchLine = true;
            offset = 6;

        } else if(curline.StartsWith("=")) {
            ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos) + i, 1 | wxSTC_FOLDLEVELHEADERFLAG);
            ctrl->SetStyling(curline.Length(), LEX_FIF_HEADER); // first 6 chars are the line number
            bytes_left = 0;
        } else if(curline == "\n") {
            // empty line
            ctrl->SetStyling(1, LEX_FIF_LINE_NUMBER); // first 6 chars are the line number
            inMatchLine = true;
            bytes_left = 0;
        } else {
            // File name
            ctrl->SetFoldLevel(ctrl->LineFromPosition(startPos) + i, 2 | wxSTC_FOLDLEVELHEADERFLAG);
            ctrl->SetStyling(curline.Length(), LEX_FIF_FILE); // first 6 chars are the line number
            bytes_left = 0;
        }

        // Check for scope
        static wxRegEx reScopeName(" \\[[\\<\\>a-z0-9_:~ ]+\\] ", wxRE_DEFAULT | wxRE_ICASE);
        size_t scopeStart = wxString::npos, scopeLen = 0;
        if(offset == 6 && reScopeName.Matches(curline)) {
            reScopeName.GetMatch(&scopeStart, &scopeLen);
            if(scopeStart == 6) {
                ctrl->SetStyling(scopeLen, LEX_FIF_SCOPE);
                bytes_left -= scopeLen;
            }
        }

        if(inMatchLine && bytes_left > 0) {
            // The remainder of this line should be a hyper link
            ctrl->SetStyling(bytes_left, LEX_FIF_MATCH);

        } else if(bytes_left > 0) {
            ctrl->SetStyling(bytes_left, LEX_FIF_DEFAULT);
        }
    }
}

void FindResultsTab::OnThemeChanged(wxCommandEvent& e)
{
    e.Skip();
    if(m_sci) {
        SetStyles(m_sci);
    } else if(m_book) {
        size_t pageCount = m_book->GetPageCount();
        for(size_t i = 0; i < pageCount; ++i) {
            wxStyledTextCtrl* stc = dynamic_cast<wxStyledTextCtrl*>(m_book->GetPage(pageCount));
            if(stc) {
                SetStyles(stc);
            }
        }
    }
}

void FindResultsTab::OnPageClosing(wxBookCtrlEvent& e)
{
    e.Skip();
    if(m_searchInProgress) {
        ::wxMessageBox(_("Can not close the search tab while a search is in progress"),
                       "CodeLite",
                       wxICON_WARNING | wxOK | wxCENTER);
        e.Veto();
        return;
    }
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
