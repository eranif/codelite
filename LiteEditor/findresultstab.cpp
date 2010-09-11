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


BEGIN_EVENT_TABLE(FindResultsTab, OutputTabWindow)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED,  FindResultsTab::OnSearchStart)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND,     FindResultsTab::OnSearchMatch)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND,      FindResultsTab::OnSearchEnded)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, FindResultsTab::OnSearchCancel)

	// Right click menu options
	EVT_MENU(XRCID("close_fif_tab"),             FindResultsTab::OnCloseTab     )
	EVT_MENU(XRCID("close_all_fif_tabs"),        FindResultsTab::OnCloseAllTabs )
	EVT_MENU(XRCID("close_other_fif_tabs"),      FindResultsTab::OnCloseOtherTab)

	EVT_UPDATE_UI(XRCID("close_fif_tab"),        FindResultsTab::OnTabMenuUI    )
	EVT_UPDATE_UI(XRCID("close_all_fif_tabs"),   FindResultsTab::OnTabMenuUI    )
	EVT_UPDATE_UI(XRCID("close_other_fif_tabs"), FindResultsTab::OnTabMenuUI    )

END_EVENT_TABLE()

FindInFilesDialog* FindResultsTab::m_find = NULL;

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name, bool useBook)
		: OutputTabWindow(parent, id, name)
		, m_searchInProgress(false)
		, m_book(NULL)
		, m_recv(NULL)
		, m_matchInfo(1)
{
	// Remove the 'find' tool
#if 0
	m_tb->RemoveTool ( XRCID ( "search_output" ) );
	m_tb->Realize();
	m_findBar->SetEditor(NULL);
#endif

	if (useBook) {

		// load the book style from the settings file
		long bookStyle = wxVB_TOP|wxVB_HAS_X|wxVB_MOUSE_MIDDLE_CLOSE_TAB|wxVB_NODND;

		m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);

		m_book->SetRightClickMenu( wxXmlResource::Get()->LoadMenu(wxT("find_in_files_right_click_menu")) );

		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(FindResultsTab::OnPageChanged), NULL, this);
		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED , NotebookEventHandler(FindResultsTab::OnPageClosed) , NULL, this);

		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_X_CLICKED,      NotebookEventHandler(FindResultsTab::OnClosePage) , NULL, this);
		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_MIDDLE_CLICKED, NotebookEventHandler(FindResultsTab::OnClosePage) , NULL, this);

		// get rid of base class scintilla component
		wxSizer *sz = m_hSizer;
		sz->Detach(m_sci);
		m_sci->Destroy();
		m_sci = NULL;
		m_findBar->SetEditor(m_sci);
#ifdef __WXMAC__
		sz->Insert(0, m_book, 1, wxALL|wxEXPAND);
#else
		sz->Add(m_book, 1, wxALL|wxEXPAND);
#endif
		sz->Layout();
	} else {
		// keep existing scintilla
		SetStyles(m_sci);
	}

	BitmapLoader &loader = *(PluginManager::Get()->GetStdIcons());

	wxTheApp->Connect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
	m_tb->AddTool ( XRCID ( "stop_search" ), wxT ( "Stop current search" ), loader.LoadBitmap(wxT("toolbars/16/build/stop")), wxT ( "Stop current search" ) );
	Connect( XRCID ( "stop_search" ), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler ( FindResultsTab::OnStopSearch  ), NULL, this );
	Connect( XRCID ( "stop_search" ), wxEVT_UPDATE_UI,             wxUpdateUIEventHandler( FindResultsTab::OnStopSearchUI), NULL, this );
	m_tb->Realize();

}

FindResultsTab::~FindResultsTab()
{
	if (m_find) {
		delete m_find;
		m_find = NULL;
	}
}

MatchInfo& FindResultsTab::GetMatchInfo(size_t idx)
{
	ListMatchInfos::iterator itMatchInfo = m_matchInfo.begin();
	if (m_book) {
		for (size_t i = 0; i < idx; ++i) {
			++itMatchInfo;
		}
	}
	return *itMatchInfo;
}

void FindResultsTab::LoadFindInFilesData()
{
	if (m_find != NULL)
		return;

	FindReplaceData data;
	EditorConfigST::Get()->ReadObject(wxT("FindInFilesData"), &data);
	m_find = new FindInFilesDialog(NULL, wxID_ANY, data);
}

void FindResultsTab::SaveFindInFilesData()
{
	if (m_find) {
		EditorConfigST::Get()->WriteObject(wxT("FindInFilesData"), &m_find->GetData());
	}
}

void FindResultsTab::SetStyles(wxScintilla *sci)
{
	InitStyle(sci, wxSCI_LEX_FIF, true);

	wxColour fifFgColour = *wxBLUE;

#ifdef __WXGTK__
	wxColour fifBgColour = DrawingUtils::GetPanelBgColour();
#else
	wxColour fifBgColour = DrawingUtils::LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 2.0);
#endif

	sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, wxT("BLACK"));
	sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_PROJECT, wxT("BLACK"));
	sci->StyleSetBackground(wxSCI_LEX_FIF_PROJECT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	sci->StyleSetBackground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_MATCH, DrawingUtils::GetTextCtrlTextColour());
	sci->StyleSetBackground(wxSCI_LEX_FIF_MATCH, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	sci->StyleSetEOLFilled (wxSCI_LEX_FIF_MATCH, true);

	sci->StyleSetForeground(wxSCI_LEX_FIF_SCOPE, wxT("GREY"));
	sci->StyleSetBackground(wxSCI_LEX_FIF_SCOPE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	sci->StyleSetEOLFilled (wxSCI_LEX_FIF_SCOPE, false);
	
	wxColour fgColour(wxT("GREEN"));

	LexerConfPtr cppLexer = EditorConfigST::Get()->GetLexer(wxT("C++"));
	if(cppLexer) {
		std::list<StyleProperty> styles = cppLexer->GetProperties();
		std::list<StyleProperty>::iterator iter = styles.begin();
		for (; iter != styles.end(); iter++) {
			if(iter->GetId() == wxSCI_C_COMMENTLINE) {
				fgColour = iter->GetFgColour();
				break;
			}
		}
	}

	sci->StyleSetForeground(wxSCI_LEX_FIF_MATCH_COMMENT, fgColour);
	sci->StyleSetBackground(wxSCI_LEX_FIF_MATCH_COMMENT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	sci->StyleSetEOLFilled (wxSCI_LEX_FIF_MATCH_COMMENT, true);

	sci->StyleSetForeground(wxSCI_LEX_FIF_FILE, DrawingUtils::GetTextCtrlTextColour());
	sci->StyleSetBackground(wxSCI_LEX_FIF_FILE, fifBgColour);
	sci->StyleSetEOLFilled (wxSCI_LEX_FIF_FILE, true);
	
	sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, DrawingUtils::GetTextCtrlTextColour());
	sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	sci->StyleSetEOLFilled(wxSCI_LEX_FIF_DEFAULT, true);
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	wxFont bold(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD);

	sci->StyleSetFont(wxSCI_LEX_FIF_FILE,          font);
	sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT,       bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_PROJECT,       bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_MATCH,         font);
	sci->StyleSetFont(wxSCI_LEX_FIF_FILE_SHORT,    font);
	sci->StyleSetFont(wxSCI_LEX_FIF_SCOPE,         font);
	sci->StyleSetFont(wxSCI_LEX_FIF_MATCH_COMMENT, font);

	sci->StyleSetHotSpot(wxSCI_LEX_FIF_MATCH,         true);
	sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE,          true);
	sci->StyleSetHotSpot(wxSCI_LEX_FIF_MATCH_COMMENT, true);

	sci->MarkerDefine       (7, wxSCI_MARK_ARROW);
	sci->MarkerSetBackground(7, *wxBLACK);

	sci->IndicatorSetForeground(1, wxT("FOREST GREEN"));

	sci->SetMarginWidth(0, 0);
	sci->SetMarginWidth(1, 16);
	sci->SetMarginWidth(2, 0);
	sci->SetMarginWidth(3, 0);
	sci->SetMarginWidth(4, 0);

	sci->SetMarginSensitive(1, true);
}

size_t FindResultsTab::GetPageCount() const
{
	return m_book->GetPageCount();
}

void FindResultsTab::AppendText(const wxString& line)
{
	wxScintilla *save = NULL;
	if (m_recv) {
		// so OutputTabWindow::AppendText() writes to the correct page
		save  = m_sci;
		m_sci = m_recv;
	}
	OutputTabWindow::AppendText(line);
	if (save) {
		m_sci = save;
	}
}

void FindResultsTab::Clear()
{
	MatchInfo& matchInfo = GetMatchInfo(m_book ? m_book->GetSelection() : 0);
	matchInfo.clear();
	OutputTabWindow::Clear();
}

void FindResultsTab::OnPageChanged(NotebookEvent& e)
{
	// this function can't be called unless m_book != NULL
	m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
	m_findBar->SetEditor(m_sci);
	if(m_sci){
		m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSCI_WRAP_WORD);
	}
}

void FindResultsTab::OnPageClosed(NotebookEvent& e)
{
	// this function can't be called unless m_book != NULL
	size_t sel = e.GetSelection();
	if(sel != Notebook::npos) {
		ListMatchInfos::iterator itMatchInfo = m_matchInfo.begin();
		for (size_t i = 0; i < e.GetSelection(); ++i) {
			++itMatchInfo;
		}
		m_matchInfo.erase(itMatchInfo);

	} else if(m_book->GetPageCount()) {
		m_matchInfo.clear();
	}

	// Create a page if there is no more
	if (m_book->GetPageCount() == 0) {
		m_sci = NULL;
	} else {
		m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
	}
	m_findBar->SetEditor(m_sci);
}

void FindResultsTab::OnFindInFiles(wxCommandEvent &e)
{
	LoadFindInFilesData();

	if (m_searchInProgress) {
		wxMessageBox(_("The search thread is currently busy"), wxT("CodeLite"), wxICON_INFORMATION|wxOK);
		return;
	}

	wxString rootDir = e.GetString();
	if (!rootDir.IsEmpty()) {
		m_find->SetRootDir(rootDir);
	}

	if (m_find->IsShown() == false) m_find->Show();
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
	m_searchInProgress = true;
	SearchData *data = (SearchData*) e.GetClientData();
	wxString label = data ? data->GetFindString() : wxT("");

	if (e.GetInt() != 0 || m_sci == NULL) {
		if (m_book) {
			wxWindowUpdateLocker locker(this);
			wxScintilla *sci = new wxScintilla(m_book);
			SetStyles(sci);

			// Make sure we can add more tabs, if not delete the last used tab and then add
			// a new tab

			long MaxBuffers(15);
			EditorConfigST::Get()->GetLongValue(wxT("MaxOpenedTabs"), MaxBuffers);

			if( (long)m_book->GetPageCount() >= MaxBuffers ) {
				// We have reached the limit of the number of open buffers
				// Close the last used buffer
				const wxArrayPtrVoid &arr = m_book->GetHistory();
				if ( arr.GetCount() ) {
					wxWindow *tab = static_cast<wxWindow*>(arr.Item(arr.GetCount()-1));
					m_book->DeletePage( m_book->GetPageIndex(tab) );
				}
			}

			m_book->AddPage(sci, label, true);
#ifdef __WXMAC__
			m_book->GetSizer()->Layout();
#endif
			size_t where = m_book->GetPageCount() - 1;

			// keep the search data used for this tab
			wxWindow *tab = m_book->GetPage(where);
			if (tab) {
				tab->SetClientData(data);
			}

			m_matchInfo.push_back(MatchInfo());
			m_sci = sci;
		}
	} else if (m_book) {
		// using current tab, update the tab title and the search data
		size_t where = m_book->GetPageIndex(m_sci);
		if (where != Notebook::npos) {
			m_book->SetPageText(where, label);
			// delete the old search data
			wxWindow *tab = m_book->GetPage(where);
			SearchData *oldData = (SearchData *) tab->GetClientData();
			if (oldData) {
				delete oldData;
			}
			// set the new search data
			tab->SetClientData(data);
		}
	}

#if wxVERSION_NUMBER >= 2900
		// This is needed in >=wxGTK-2.9, otherwise the 'Search' pane doesn't fully expand
		SendSizeEvent(wxSEND_EVENT_POST);
#endif

	m_recv = m_sci;
	Clear();

	if (data) {
		m_searchData = *data;

		wxString message;
		message << wxT("====== Searching for: '") <<  data->GetFindString()
		<< wxT("'; Match case: ")         << (data->IsMatchCase()         ? wxT("true") : wxT("false"))
		<< wxT(" ; Match whole word: ")   << (data->IsMatchWholeWord()    ? wxT("true") : wxT("false"))
		<< wxT(" ; Regular expression: ") << (data->IsRegularExpression() ? wxT("true") : wxT("false"))
		<< wxT(" ======\n");
		AppendText( message );
	}
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
	SearchResultList *res = (SearchResultList*) e.GetClientData();
	if (!res)
		return;

	size_t m = m_book ? m_book->GetPageIndex(m_recv) : 0;
	if(m == Notebook::npos) {
		delete res;
		return;
	}

	MatchInfo& matchInfo = GetMatchInfo(m);
	for (SearchResultList::iterator iter = res->begin(); iter != res->end(); iter++) {
		if (matchInfo.empty() || matchInfo.rbegin()->second.GetFileName() != iter->GetFileName()) {
			wxFileName fn(iter->GetFileName());
			fn.MakeRelativeTo();
			
			AppendText(fn.GetFullPath() + wxT("\n"));
		}
				
		int lineno = m_recv->GetLineCount()-1;
		matchInfo.insert(std::make_pair(lineno, *iter));
		wxString text = iter->GetPattern();
		int delta = -text.Length();
		text.Trim(false);
		delta += text.Length();
		text.Trim();

		wxString linenum;
		if(iter->GetMatchState() == CppWordScanner::STATE_CPP_COMMENT || iter->GetMatchState() == CppWordScanner::STATE_C_COMMENT)
			linenum = wxString::Format(wxT(".%4u: "), iter->GetLineNumber());
		else
			linenum = wxString::Format(wxT(" %4u: "), iter->GetLineNumber());
		
		SearchData d = GetSearchData(m_recv);
		// Print the scope name
		if (d.GetDisplayScope()) {
			TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->GetFileName(), iter->GetLineNumber());
			wxString scopeName (wxT("global"));
			if(tag) {
				scopeName = tag->GetPath();
			}
			
			linenum << wxT("[ ") << scopeName << wxT(" ] ");
			iter->SetScope(scopeName);
		}
		
		delta += linenum.Length();
		AppendText(linenum + text + wxT("\n"));
		m_recv->IndicatorFillRange(m_sci->PositionFromLine(lineno)+iter->GetColumn()+delta, iter->GetLen());
	}
	delete res;
}

void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
{
	m_searchInProgress = false;
	SearchSummary *summary = (SearchSummary*) e.GetClientData();
	if (!summary)
		return;

	// did the page closed before the search ended?
	if(m_book && m_book->GetPageIndex(m_recv) != Notebook::npos) {

		AppendText(summary->GetMessage());
		m_recv = NULL;
		if (m_tb->GetToolState(XRCID("scroll_on_output"))) {
			m_sci->GotoLine(0);
		}

	}
	delete summary;
}

void FindResultsTab::OnSearchCancel(wxCommandEvent &e)
{
	m_searchInProgress = false;
	wxString *str = (wxString*) e.GetClientData();
	if (!str)
		return;

	// did the page closed before the search ended?
	if(m_book && m_book->GetPageIndex(m_recv) != Notebook::npos) {
		AppendText(*str + wxT("\n"));
	}

	delete str;
	m_recv = NULL;
}

void FindResultsTab::OnClearAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	if (m_recv != NULL) {
		SearchThreadST::Get()->StopSearch();
	}
	Clear();
}

void FindResultsTab::OnClearAllUI(wxUpdateUIEvent& e)
{
	e.Enable(m_recv != NULL || (m_sci && m_sci->GetLength() > 0));
}

void FindResultsTab::OnRepeatOutput(wxCommandEvent &e)
{
	wxUnusedVar(e);

	if (m_book) {
		size_t sel = m_book->GetSelection();
		if (sel != Notebook::npos) {
			// get the search data used to generate the output on the selected tab
			wxWindow *tab = m_book->GetPage(sel);
			if (tab) {
				SearchData *searchData = (SearchData *)tab->GetClientData();
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

void FindResultsTab::OnMouseDClick(wxScintillaEvent &e)
{
	long pos = e.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	int style = m_sci->GetStyleAt(pos);

	if (style == wxSCI_LEX_FIF_FILE || style == wxSCI_LEX_FIF_PROJECT) {
		m_sci->ToggleFold(line);
	} else {
		size_t n = m_book ? m_book->GetSelection() : 0;
		const MatchInfo& matchInfo = GetMatchInfo(n);
		MatchInfo::const_iterator m = matchInfo.find(line);
		if (m != matchInfo.end()) {
			DoOpenSearchResult( m->second, m_sci, m->first );
		}
	}

	m_sci->SetSelection(wxNOT_FOUND, pos);
}

long FindResultsTab::GetBookStyle()
{
	return 0;
}

SearchData FindResultsTab::GetSearchData(wxScintilla* sci)
{
	if (m_book) {
		size_t i = m_book->GetPageIndex(sci);
		if (i != Notebook::npos) {
			wxWindow *tab = m_book->GetPage(i);
			if (tab) {
				SearchData *data = (SearchData *) tab->GetClientData();
				if (data) {
					return *data;
				}
			}
		}
		return SearchData();
	} else {
		// in case we dont have a notebook (e.g. 'Replace In Files')
		// we use the global saved search data
		return m_searchData;
	}
}

void FindResultsTab::OnCloseAllTabs(wxCommandEvent& e)
{
	wxUnusedVar( e );
	if (m_book) {
		m_book->DeleteAllPages(true);
	}
}

void FindResultsTab::OnCloseOtherTab(wxCommandEvent& e)
{
	wxUnusedVar( e );
	if (m_book) {
		size_t idx = m_book->GetSelection();
		if (idx != Notebook::npos) {
			for (size_t i=0; i<idx; i++) {
				m_book->DeletePage((size_t)0);
			}

			size_t number = m_book->GetPageCount();
			for (size_t i = number - 1; i>0; i--) {
				m_book->DeletePage(i);
			}
		}
	}
}

void FindResultsTab::OnCloseTab(wxCommandEvent& e)
{
	wxUnusedVar( e );
	if (m_book) {
		size_t idx = m_book->GetSelection();
		if (idx != Notebook::npos) {
			m_book->DeletePage(idx);
		}
	}
}

void FindResultsTab::OnTabMenuUI(wxUpdateUIEvent& e)
{
	e.Enable( !m_searchInProgress );
}

void FindResultsTab::NextMatch()
{
	// m_sci holds to the selected tab's scintilla editor
	if ( m_sci ) {
		const MatchInfo& matchInfo = GetMatchInfo( m_book ? m_book->GetSelection() : 0 );

		// locate the last match
		int firstLine = m_sci->MarkerNext(0, 255);
		if ( firstLine == wxNOT_FOUND ) {
			firstLine = 0;
		}

		// We found the last marker
		for (int i=firstLine+1; i<m_sci->GetLineCount(); i++) {

			// Find the next match
			MatchInfo::const_iterator iter = matchInfo.find(i);
			if ( iter != matchInfo.end() ) {
				SearchResult sr = iter->second;

				// open the new searchresult in the editor
				DoOpenSearchResult ( sr, m_sci, i );
				return;
			}
		}
		// if we are here, it means we are the end of the search results list, add a status message
		wxCommandEvent e(wxEVT_UPDATE_STATUS_BAR);
		e.SetEventObject(this);
		e.SetString(wxString::Format(wxT("Reached the end of 'find in files' search results list" )));
		e.SetInt(0);
		clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
	}
}

void FindResultsTab::PrevMatch()
{
	// m_sci holds to the selected tab's scintilla editor
	if ( m_sci ) {
		const MatchInfo& matchInfo = GetMatchInfo( m_book ? m_book->GetSelection() : 0 );

		// locate the last match
		int firstLine = m_sci->MarkerPrevious(m_sci->GetLineCount()-1, 255);
		if ( firstLine == wxNOT_FOUND ) {
			firstLine = m_sci->GetLineCount();
		}

		// We found the last marker
		for (int i=firstLine-1; i>=0; i--) {

			// Find the next match
			MatchInfo::const_iterator iter = matchInfo.find(i);
			if ( iter != matchInfo.end() ) {
				SearchResult sr = iter->second;

				// open the new searchresult in the editor
				DoOpenSearchResult ( sr, m_sci, i );
				return;
			}
		}
		// if we are here, it means we are the top of the search results list, add a status message
		wxCommandEvent e(wxEVT_UPDATE_STATUS_BAR);
		e.SetEventObject(this);
		e.SetString(wxString::Format(wxT("Reached the begining of 'find in files' search results list" )));
		e.SetInt(0);
		clMainFrame::Get()->GetEventHandler()->AddPendingEvent(e);
	}
}

void FindResultsTab::DoOpenSearchResult(const SearchResult &result, wxScintilla *sci, int markerLine)
{
	if (!result.GetFileName().IsEmpty()) {
		LEditor *editor = clMainFrame::Get()->GetMainBook()->OpenFile(result.GetFileName(), wxEmptyString, result.GetLineNumber()-1);
		if (editor && result.GetColumn() >= 0 && result.GetLen() >= 0) {

			int offset = editor->PositionFromLine(result.GetLineNumber()-1) + result.GetColumn();

			editor->SetCaretAt(offset);
			editor->GotoLine(result.GetLineNumber()-1);
			editor->EnsureCaretVisible();
			editor->SetSelection(offset, offset + result.GetLen());

#ifdef __WXGTK__
			editor->ScrollToColumn(0);
#endif

			if ( sci ) {
				// remove the previous marker and add the new one
				sci->MarkerDeleteAll( 7 );
				sci->MarkerAdd(markerLine, 7 );

				// make the marked line visible
				int pos = sci->PositionFromLine(markerLine);
				sci->SetCurrentPos     (pos);
				sci->SetSelectionStart (pos);
				sci->SetSelectionEnd   (pos);
				sci->EnsureCaretVisible(   );
#ifdef __WXGTK__
				sci->ScrollToColumn(0);
#endif
			}
		}
	}
}

void FindResultsTab::OnStopSearch(wxCommandEvent& e)
{
	// stop the search thread
	SearchThreadST::Get()->StopSearch();
}

void FindResultsTab::OnStopSearchUI(wxUpdateUIEvent& e)
{
	e.Enable(m_searchInProgress);
}

void FindResultsTab::OnClosePage(NotebookEvent& e)
{
	int where = e.GetSelection();
	if(where == wxNOT_FOUND) {
		return;
	}
	m_book->DeletePage((size_t)where, true);
}
