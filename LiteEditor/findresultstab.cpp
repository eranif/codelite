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

#include "manager.h"
#include "frame.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "findresultstab.h"


#ifndef wxScintillaEventHandler
#define wxScintillaEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxScintillaEventFunction, &func)
#endif

extern void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b);
extern void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l);

// TODO: move this somewhere more appropriate
static wxColour lightColour(wxColour color, float level)
{
	float h, s, l, r, g, b;
	RGB_2_HSL(color.Red(), color.Green(), color.Blue(), &h, &s, &l);
	HSL_2_RGB(h, s, l + std::max(level/20.0, 0.0), &r, &g, &b);
	return wxColour((unsigned char)r, (unsigned char)g, (unsigned char)b);
}

FindInFilesDialog* FindResultsTab::m_find = NULL;

FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name, size_t numpages)
		: OutputTabWindow(parent, id, name)
		, m_book(NULL)
		, m_recv(NULL)
{
	m_tb->AddTool(XRCID("collapse_all"), _("Fold All Results"),
	              wxXmlResource::Get()->LoadBitmap(wxT("fold_airplane")),
	              _("Fold All Results"));
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FindResultsTab::OnCollapseAll ), NULL, this);
	m_tb->AddTool(XRCID("repeat_search"), _("Repeat Search"),
	              wxXmlResource::Get()->LoadBitmap(wxT("find_refresh")),
	              _("Repeat Search"));
	Connect(XRCID("repeat_search"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FindResultsTab::OnRepeatSearch), NULL, this);
	m_tb->Realize();
	m_tb->EnableTool(XRCID("repeat_search"), false);

	m_matchInfo.resize(numpages);
	if (numpages > 1) {
		m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_NODND);
		for (size_t i = 1; i <= numpages; i++) {
			wxScintilla *sci = new wxScintilla(m_book);
			SetStyles(sci);
			sci->Connect(wxEVT_SCI_MARGINCLICK, wxScintillaEventHandler(FindResultsTab::OnMarginClick), NULL, this);
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

	Connect(wxEVT_SCI_MARGINCLICK, wxScintillaEventHandler(FindResultsTab::OnMarginClick), NULL, this);

	Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED,  wxCommandEventHandler(FindResultsTab::OnSearchStart),  NULL, this);
	Connect(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND,     wxCommandEventHandler(FindResultsTab::OnSearchMatch),  NULL, this);
	Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND,      wxCommandEventHandler(FindResultsTab::OnSearchEnded),  NULL, this);
	Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEventHandler(FindResultsTab::OnSearchCancel), NULL, this);

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

static void DefineMarker(wxScintilla *sci, int marker, int markerType, wxColor fore, wxColor back)
{
	sci->MarkerDefine(marker, markerType);
	sci->MarkerSetForeground(marker, fore);
	sci->MarkerSetBackground(marker, back);
}

void FindResultsTab::SetStyles(wxScintilla *sci)
{
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	wxFont bold(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxFONTWEIGHT_BOLD);

	sci->SetLexer(wxSCI_LEX_FIF);  // use custom FIF (Find In File) lexer
	sci->StyleClearAll();

	sci->StyleSetFont(0, font);
	sci->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	sci->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

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

	sci->StyleSetFont(wxSCI_LEX_FIF_FILE,       font);
	sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_PROJECT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_MATCH,      font);
	sci->StyleSetFont(wxSCI_LEX_FIF_FILE_SHORT, font);
	sci->StyleSetFont(wxSCI_LEX_FIF_SCOPE,      font);

	sci->StyleSetHotSpot(wxSCI_LEX_FIF_MATCH, true);
	sci->StyleSetHotSpot(wxSCI_LEX_FIF_FILE,  true);

	sci->SetHotspotActiveUnderline (false);
	sci->SetHotspotActiveBackground(true, lightColour(wxT("BLUE"), 8.0));

	sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	sci->SetMarginWidth(2, 0);
	sci->SetMarginWidth(1, 0);
	sci->SetMarginWidth(0, 0);

	// enable folding
	sci->SetMarginMask(4, wxSCI_MASK_FOLDERS);
	sci->SetMarginWidth(4, 16);
	sci->SetProperty(wxT("fold"), wxT("1"));
	sci->SetMarginSensitive(4, true);

	// mark folded lines with line
	sci->SetFoldFlags(16);

	// make the fold lines grey
	sci->StyleSetForeground(wxSCI_STYLE_DEFAULT, wxT("GREY"));

	wxColor fore(0xff, 0xff, 0xff);
	wxColor back(0x80, 0x80, 0x80);

	DefineMarker(sci, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_ARROWDOWN,  fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_ARROW,      fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_ARROW,      fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN,  fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, fore, back);

	sci->SetIndicatorCurrent(1);
	sci->IndicatorSetForeground(1, wxT("GOLD"));
#ifdef __WXMAC__
	// Different settings for Mac
	sci->IndicatorSetUnder(1, false);
	sci->IndicatorSetStyle(1, wxSCI_INDIC_BOX);
#else
	sci->IndicatorSetUnder(1, true);
	sci->IndicatorSetStyle(1, wxSCI_INDIC_ROUNDBOX);
#endif

	sci->SetReadOnly(true);
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
	m_tb->EnableTool(XRCID("repeat_search"), false);
	m_matchInfo[m_book ? m_book->GetSelection() : 0].clear();
	OutputTabWindow::Clear();
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

void FindResultsTab::OnRepeatSearch(wxCommandEvent &e)
{
	wxUnusedVar(e);
	SearchThreadST::Get()->PerformSearch(m_data);
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
	ManagerST::Get()->ShowOutputPane(m_name); // derived classes may change name
	if (m_book) {
		m_book->SetSelection(e.GetInt());
	}
	m_recv = m_sci;
	Clear();
	m_tb->EnableTool(XRCID("clear_all_output"), false);

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
	m_tb->EnableTool(XRCID("repeat_search"), true);
	m_tb->EnableTool(XRCID("clear_all_output"), true);
}

void FindResultsTab::OnSearchCancel(wxCommandEvent &e)
{
	wxString *str = (wxString*) e.GetClientData();
	if (!str)
		return;
	AppendText(*str + wxT("\n"));
	delete str;
	m_recv = NULL;
	m_tb->EnableTool(XRCID("clear_all_output"), true);
}

void FindResultsTab::OnPageChanged(NotebookEvent& e)
{
	// this function can't be called unless m_book != NULL
	m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
	m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSCI_WRAP_WORD);
}

void FindResultsTab::OnHotspotClicked(wxScintillaEvent &e)
{
	OnMouseDClick(e);
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

	m_sci->SetCurrentPos(pos);
	m_sci->SetSelectionStart(pos);
	m_sci->SetSelectionEnd(pos);
}

void FindResultsTab::OnMarginClick(wxScintillaEvent& e)
{
	if (e.GetMargin() == 4) {
		m_sci->ToggleFold(m_sci->LineFromPosition(e.GetPosition()));
	}
}

void FindResultsTab::OnCollapseAll(wxCommandEvent& e)
{
	// go through the whole document, toggling folds that are expanded
	int maxLine = m_sci->GetLineCount();
	for (int line = 0; line < maxLine; line++) {  // For every line
		int level = m_sci->GetFoldLevel(line);
		// The next statement means: If this level is a Fold start
		if (level & wxSCI_FOLDLEVELHEADERFLAG) {
			if ( m_sci->GetFoldExpanded(line) == true ) m_sci->ToggleFold( line );
		}
	}
}


size_t FindResultsTab::GetPageCount() const
{
	return m_book->GetPageCount();
}
