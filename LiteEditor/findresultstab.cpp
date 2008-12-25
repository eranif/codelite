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
#include <wx/tokenzr.h>
#include "globals.h"
#include "manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "globals.h"
#include "findresultstab.h"


BEGIN_EVENT_TABLE(FindResultsTab, OutputTabWindow)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED,  FindResultsTab::OnSearchStart)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND,     FindResultsTab::OnSearchMatch)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND,      FindResultsTab::OnSearchEnded)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, FindResultsTab::OnSearchCancel)
END_EVENT_TABLE()


FindInFilesDialog* FindResultsTab::m_find = NULL;

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name, size_t numpages)
    : OutputTabWindow(parent, id, name)
    , m_book(NULL)
    , m_recv(NULL)
{
	m_matchInfo.resize(numpages);
	if (numpages > 1) {
		m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_NODND|wxVB_BG_GRADIENT|wxVB_TAB_DECORATION);
		for (size_t i = 1; i <= numpages; i++) {
			wxScintilla *sci = new wxScintilla(m_book);
			SetStyles(sci);
			m_book->AddPage(sci, wxString::Format(wxT("Find Results %u"), i));
		}
		m_book->SetSelection(size_t(0));
		m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(FindResultsTab::OnPageChanged), NULL, this);

		// get rid of base class scintilla component
		wxSizer *sz = GetSizer();
		sz->Detach(m_sci);
		m_sci->Destroy();

		// use base class scintilla ptr as ref to currently selected Find Results tab
		// so that base class functions (eg AppendText) go to the correct tab
		m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());

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

void FindResultsTab::LoadFindInFilesData()
{
	if (m_find != NULL)
		return;

	FindReplaceData data;
	EditorConfigST::Get()->ReadObject(wxT("FindInFilesData"), &data);
	m_find = new FindInFilesDialog(NULL, wxID_ANY, data, Frame::Get()->GetOutputPane()->GetFindResultsTab()->GetPageCount());
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
	m_matchInfo[m_book ? m_book->GetSelection() : 0].clear();
	OutputTabWindow::Clear();
}

void FindResultsTab::OnPageChanged(NotebookEvent& e)
{
	// this function can't be called unless m_book != NULL
	m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
	m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSCI_WRAP_WORD);
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
	if (m_book) {
		m_book->SetSelection(e.GetInt());
	}
	m_recv = m_sci;
	Clear();

	SearchData *data = (SearchData*) e.GetClientData();
	m_data = data ? *data : SearchData();
	delete data;

	wxString message;
	message << wxT("====== Searching for: '") <<  m_data.GetFindString()
	<< wxT("'; Match case: ")         << (m_data.IsMatchCase()         ? wxT("true") : wxT("false"))
	<< wxT(" ; Match whole word: ")   << (m_data.IsMatchWholeWord()    ? wxT("true") : wxT("false"))
	<< wxT(" ; Regular expression: ") << (m_data.IsRegularExpression() ? wxT("true") : wxT("false"))
	<< wxT(" ======\n");
	AppendText(message);
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
	SearchResultList *res = (SearchResultList*) e.GetClientData();
	if (!res)
		return;
	int m = m_book ? m_book->GetPageIndex(m_recv) : 0;
	for (SearchResultList::iterator iter = res->begin(); iter != res->end(); iter++) {
		if (m_matchInfo[m].empty() || m_matchInfo[m].rbegin()->second.GetFileName() != iter->GetFileName()) {
			AppendText(iter->GetFileName() + wxT("\n"));
		}
		int lineno = m_recv->GetLineCount()-1;
		m_matchInfo[m].insert(std::make_pair(lineno, *iter));
		wxString text = iter->GetPattern();
		int delta = -text.Length();
		text.Trim(false);
		delta += text.Length();
		text.Trim();
		wxString linenum = wxString::Format(wxT(" %4u: "), iter->GetLineNumber());
		if (m_data.GetDisplayScope()) {
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
    e.Enable(m_recv != NULL || m_sci->GetLength() > 0);
}

void FindResultsTab::OnRepeatOutput(wxCommandEvent &e)
{
	wxUnusedVar(e);
	SearchThreadST::Get()->PerformSearch(m_data);
}

void FindResultsTab::OnRepeatOutputUI(wxUpdateUIEvent& e)
{
    e.Enable(m_recv == NULL && m_sci->GetLength() > 0);
}

void FindResultsTab::OnMouseDClick(wxScintillaEvent &e)
{
	long pos = e.GetPosition();
	int line = m_sci->LineFromPosition(pos);
	int style = m_sci->GetStyleAt(pos);

	if (style == wxSCI_LEX_FIF_FILE || style == wxSCI_LEX_FIF_PROJECT) {
		m_sci->ToggleFold(line);
	} else {
		LEditor *currentEditor = ManagerST::Get()->GetActiveEditor();
		int n = m_book ? m_book->GetSelection() : 0;
		std::map<int,SearchResult>::iterator m = m_matchInfo[n].find(line);
		if (m != m_matchInfo[n].end() && !m->second.GetFileName().IsEmpty()) {
			LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(m->second.GetFileName(), wxEmptyString, m->second.GetLineNumber()-1);
			if (editor && m->second.GetColumn() >= 0 && m->second.GetLen() >= 0) {

				int offset = editor->PositionFromLine(m->second.GetLineNumber()-1) + m->second.GetColumn();
				editor->SetSelection(offset, offset + m->second.GetLen());

				if (currentEditor) {
					// create a browsing record
					currentEditor->AddBrowseRecord(NULL);
				}
			}
		}
	}

	m_sci->SetSelection(wxNOT_FOUND, pos);
}

