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
#include "drawingutils.h"
#include "custom_tabcontainer.h"
#include "custom_tab.h"
#include <wx/tokenzr.h>
#include "globals.h"
#include "manager.h"
#include "frame.h"
#include "ctags_manager.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "globals.h"
#include "findresultstab.h"


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
	if (useBook) {

		// load the book style from the settings file
		long bookStyle = wxVB_BOTTOM|wxVB_NODND|wxVB_HAS_X|wxVB_MOUSE_MIDDLE_CLOSE_TAB|wxVB_FIXED_WIDTH;
		EditorConfigST::Get()->GetLongValue(wxT("FindResults"), bookStyle);

		m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, bookStyle);
		m_book->SetFixedTabWidth(120);

		m_book->SetRightClickMenu( wxXmlResource::Get()->LoadMenu(wxT("find_in_files_right_click_menu")) );

		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(FindResultsTab::OnPageChanged), NULL, this);
		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CLOSED , NotebookEventHandler(FindResultsTab::OnPageClosed) , NULL, this);

		// get rid of base class scintilla component
		wxSizer *sz = GetSizer();
		sz->Detach(m_sci);
		m_sci->Destroy();
		m_sci = NULL;

		sz->Add(m_book, 1, wxALL|wxEXPAND);
		sz->Layout();
	} else {
		// keep existing scintilla
		SetStyles(m_sci);
	}

	wxTheApp->Connect(XRCID("find_in_files"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FindResultsTab::OnFindInFiles), NULL, this);
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
	// assert(idx < m_matchInfo.size());
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

	sci->StyleSetForeground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	sci->StyleSetBackground(wxSCI_LEX_FIF_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_PROJECT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	sci->StyleSetBackground(wxSCI_LEX_FIF_PROJECT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_FILE, wxT("BLUE"));
	sci->StyleSetBackground(wxSCI_LEX_FIF_FILE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW));
	sci->StyleSetBackground(wxSCI_LEX_FIF_FILE_SHORT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_MATCH, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
	sci->StyleSetBackground(wxSCI_LEX_FIF_MATCH, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	sci->StyleSetForeground(wxSCI_LEX_FIF_SCOPE, wxT("PURPLE"));
	sci->StyleSetBackground(wxSCI_LEX_FIF_SCOPE, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	wxFont bold(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD);

	sci->StyleSetFont(wxSCI_LEX_FIF_FILE,       font);
	sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_PROJECT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_MATCH,      font);
	sci->StyleSetFont(wxSCI_LEX_FIF_FILE_SHORT, font);
	sci->StyleSetFont(wxSCI_LEX_FIF_SCOPE,      font);

	sci->StyleSetHotSpot(wxSCI_LEX_FIF_MATCH, true);
	sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE,  true);

	sci->MarkerDefine       (7, wxSCI_MARK_BACKGROUND);
	sci->MarkerSetBackground(7, DrawingUtils::LightColour(wxT("BLUE"), 8));
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
	m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSCI_WRAP_WORD);
}

void FindResultsTab::OnPageClosed(NotebookEvent& e)
{
	// this function can't be called unless m_book != NULL
	ListMatchInfos::iterator itMatchInfo = m_matchInfo.begin();
	for (size_t i = 0; i < e.GetSelection(); ++i) {
		++itMatchInfo;
	}
	m_matchInfo.erase(itMatchInfo);

	// Create a page if there is no more
	if (m_book->GetPageCount() == 0) {
		m_sci = NULL;
	}
}

void FindResultsTab::OnFindInFiles(wxCommandEvent &e)
{
	LoadFindInFilesData();

	if (m_recv) {
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
			wxScintilla *sci = new wxScintilla(m_book);
			SetStyles(sci);

			// add new page
			m_book->AddPage(sci, label, label, wxNullBitmap, true);
			size_t where = m_book->GetPageCount() - 1;

			// keep the search data used for this tab
			CustomTab *tab = m_book->GetTabContainer()->IndexToTab(where);
			if (tab) {
				tab->SetUserData(data);
			}

			m_matchInfo.push_back(MatchInfo());
			m_sci = sci;
		}
	} else if (m_book) {
		// using current tab, update the tab title and the search data
		size_t where = m_book->GetPageIndex(m_sci);
		if (where != Notebook::npos) {
			m_book->SetPageText(where, label, label);
			// delete the old search data
			CustomTab *tab = m_book->GetTabContainer()->IndexToTab(where);
			SearchData *oldData = (SearchData *)tab->GetUserData();
			if (oldData) {
				delete oldData;
			}
			// set the new search data
			tab->SetUserData(data);
		}
	}

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
	MatchInfo& matchInfo = GetMatchInfo(m);
	for (SearchResultList::iterator iter = res->begin(); iter != res->end(); iter++) {
		if (matchInfo.empty() || matchInfo.rbegin()->second.GetFileName() != iter->GetFileName()) {
			AppendText(iter->GetFileName() + wxT("\n"));
		}
		int lineno = m_recv->GetLineCount()-1;
		matchInfo.insert(std::make_pair(lineno, *iter));
		wxString text = iter->GetPattern();
		int delta = -text.Length();
		text.Trim(false);
		delta += text.Length();
		text.Trim();
		wxString linenum = wxString::Format(wxT(" %4u: "), iter->GetLineNumber());

		SearchData d = GetSearchData(m_recv);
		if (d.GetDisplayScope()) {
			TagEntryPtr tag = TagsManagerST::Get()->FunctionFromFileLine(iter->GetFileName(), iter->GetLineNumber());
			if (tag) {
				linenum << wxT("[") << tag->GetPath() << wxT("] ");
			}
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

	AppendText(summary->GetMessage());
	delete summary;
	m_recv = NULL;
	if (m_tb->GetToolState(XRCID("scroll_on_output"))) {
		m_sci->GotoLine(0);
	}
}

void FindResultsTab::OnSearchCancel(wxCommandEvent &e)
{
	m_searchInProgress = false;
	wxString *str = (wxString*) e.GetClientData();
	if (!str)
		return;
	AppendText(*str + wxT("\n"));
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
			CustomTab *tab = m_book->GetTabContainer()->IndexToTab(sel);
			if (tab) {
				SearchData *searchData = (SearchData *)tab->GetUserData();
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
	return m_book->GetBookStyle();
}

SearchData FindResultsTab::GetSearchData(wxScintilla* sci)
{
	if (m_book) {
		size_t i = m_book->GetPageIndex(sci);
		if (i != Notebook::npos) {
			CustomTab *tab = m_book->GetTabContainer()->IndexToTab(i);
			if (tab) {
				SearchData *data = (SearchData *) tab->GetUserData();
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
		Frame::Get()->AddPendingEvent(e);
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
		Frame::Get()->AddPendingEvent(e);
	}
}

void FindResultsTab::DoOpenSearchResult(const SearchResult &result, wxScintilla *sci, int markerLine)
{
	if (!result.GetFileName().IsEmpty()) {
		LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(result.GetFileName(), wxEmptyString, result.GetLineNumber()-1);
		if (editor && result.GetColumn() >= 0 && result.GetLen() >= 0) {
			int offset = editor->PositionFromLine(result.GetLineNumber()-1) + result.GetColumn();
			editor->EnsureVisible(result.GetLineNumber()-1);
			editor->SetSelection(offset, offset + result.GetLen());

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
			}
		}
	}
}
