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
	return wxColour(r, g, b);
}

static void DefineMarker(wxScintilla *sci, int marker, int markerType, wxColor fore, wxColor back)
{
	sci->MarkerDefine(marker, markerType);
	sci->MarkerSetForeground(marker, fore);
	sci->MarkerSetBackground(marker, back);
}

static void SetStyles(wxScintilla *sci)
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

	sci->StyleSetFont(wxSCI_LEX_FIF_FILE,       font);
	sci->StyleSetFont(wxSCI_LEX_FIF_DEFAULT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_PROJECT,    bold);
	sci->StyleSetFont(wxSCI_LEX_FIF_MATCH,      font);
	sci->StyleSetFont(wxSCI_LEX_FIF_FILE_SHORT, font);

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
    
    sci->SetReadOnly(true);
}


FindResultsTab::fifMatchInfo::fifMatchInfo(const wxString &loc, const wxString &fileName)
    : file_name(fileName)
    , line_number(wxNOT_FOUND)
    , match_len(wxNOT_FOUND)
    , col(wxNOT_FOUND)
{
	// each line has the format of
	// filename(line, col, len): text of whole line
    // here, we have to parse the part in parentheses
	loc.BeforeFirst(wxT(',')).Trim().Trim(false).ToLong(&line_number);
	loc.AfterFirst(wxT(',')).BeforeFirst(wxT(',')).Trim().Trim(false).ToLong(&col);
	loc.AfterFirst(wxT(',')).AfterFirst(wxT(',')).BeforeFirst(wxT(')')).Trim().Trim(false).ToLong(&match_len);
    if (line_number > 0) {
        line_number--; // scintilla's line-numbering starts at 0
    }
}


FindResultsTab::FindResultsTab(wxWindow *parent, wxWindowID id, const wxString &name)
    : OutputTabWindow(parent, id, name)
    , m_book(NULL)
    , m_recv(NULL)
{
	m_tb->AddTool(XRCID("collapse_all"), _("Fold All Results"),
	              wxXmlResource::Get()->LoadBitmap(wxT("fold_airplane")),
	              _("Fold All Results"));
	Connect( XRCID("collapse_all"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( FindResultsTab::OnCollapseAll ), NULL, this);
    
    m_matchInfo.resize(5);
    
    m_book = new Notebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVB_BOTTOM|wxVB_NODND);
    for (size_t i = 0; i < m_matchInfo.size(); i++) {
        wxScintilla *sci = new wxScintilla(m_book);
        SetStyles(sci);
        sci->Connect(wxEVT_SCI_MARGINCLICK, wxScintillaEventHandler(FindResultsTab::OnMarginClick), NULL, this);
        m_book->AddPage(sci, wxString::Format(wxT("Find Results %u"), i+1));
    }
    m_book->SetSelection(size_t(0));
    m_book->Connect(wxEVT_COMMAND_BOOK_PAGE_CHANGED, NotebookEventHandler(FindResultsTab::OnPageChanged), NULL, this);
    
    // get rid of base class scintilla component
    wxSizer *sz = GetSizer();
    sz->Detach(m_sci);
    m_sci->Destroy();
    
    // use base class scintilla ptr as ref to currently selected Find Results tab
    // so that base class functions (eg AppendText) go to the correct tab
    m_sci = m_recv = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
    
    sz->Add(m_book, 1, wxALL|wxEXPAND);
	sz->Layout();
    
    Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED,  wxCommandEventHandler(FindResultsTab::OnSearchStart),  NULL, this);
    Connect(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND,     wxCommandEventHandler(FindResultsTab::OnSearchMatch),  NULL, this);
    Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND,      wxCommandEventHandler(FindResultsTab::OnSearchEnded),  NULL, this);
    Connect(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEventHandler(FindResultsTab::OnSearchCancel), NULL, this);
}

FindResultsTab::~FindResultsTab()
{
}

void FindResultsTab::AppendText(const wxString& line)
{
    if (!m_recv)
        return;
        
    m_sci = m_recv; // so OutputTabWindow::AppendText() writes to the correct page
    
    if (line.StartsWith(wxT("="))) {
        OutputTabWindow::AppendText(line + wxT("\n"));
    } else {
        wxString fileName = line.BeforeFirst(wxT('(')).Trim().Trim(false);
        wxString location = line.AfterFirst (wxT('(')).BeforeFirst(wxT(')'));
        wxString text     = line.AfterFirst (wxT(')')).AfterFirst (wxT(':')).Trim().Trim(false);
        
        int m = m_book->GetPageIndex(m_sci);
        if (m_matchInfo[m].empty() || m_matchInfo[m].rbegin()->second.file_name != fileName) {
            // new file started
            OutputTabWindow::AppendText(fileName + wxT("\n"));
        }
        fifMatchInfo info(location, fileName);
        m_matchInfo[m].insert(std::make_pair(m_sci->GetLineCount()-1, info));
        OutputTabWindow::AppendText(wxString::Format(wxT(" %4u: %s\n"), info.line_number+1, text.c_str()));
    }
    
    m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
}

void FindResultsTab::Clear()
{
	m_matchInfo[m_book->GetSelection()].clear();
	OutputTabWindow::Clear();
}

void FindResultsTab::OnSearchStart(wxCommandEvent& e)
{
    ManagerST::Get()->ShowOutputPane(Frame::Get()->GetOutputPane()->GetCaption());
    m_book->SetSelection(e.GetInt());
    Clear();
    m_recv = m_sci;
    wxString *str = (wxString*) e.GetClientData();
    if (str) {
        AppendText(*str);
        delete str;
    }
}

void FindResultsTab::OnSearchMatch(wxCommandEvent& e)
{
    SearchResultList *res = (SearchResultList*) e.GetClientData();
    if (res) {
        for (SearchResultList::iterator iter = res->begin(); iter != res->end(); iter++) {
            AppendText(iter->GetMessage());
        }
        delete res;
    }
}

void FindResultsTab::OnSearchEnded(wxCommandEvent& e)
{
    SearchSummary *summary = (SearchSummary*) e.GetClientData();
    if (summary) {
        AppendText(summary->GetMessage());
        delete summary;
    }
}

void FindResultsTab::OnSearchCancel(wxCommandEvent &e)
{
    wxString *str = (wxString*) e.GetClientData();
    if (str) {
        AppendText(*str + wxT("\n"));
        delete str;
    }
}

void FindResultsTab::OnPageChanged(NotebookEvent& e)
{
    m_sci = dynamic_cast<wxScintilla*>(m_book->GetCurrentPage());
    m_tb->ToggleTool(XRCID("word_wrap_output"), m_sci->GetWrapMode() == wxSCI_WRAP_WORD);
}

void FindResultsTab::OnHotspotClicked(wxScintillaEvent &event)
{
    OnMouseDClick(event);
}

void FindResultsTab::OnMouseDClick(wxScintillaEvent &event)
{
	long pos = event.GetPosition();
    int line = m_sci->LineFromPosition(pos);
    int style = m_sci->GetStyleAt(pos);
    
	if (style == wxSCI_LEX_FIF_FILE || style == wxSCI_LEX_FIF_PROJECT) {
		m_sci->ToggleFold(line);
	} else {
        int n = m_book->GetSelection();
        std::map<int,fifMatchInfo>::iterator m = m_matchInfo[n].find(line);
        if (m != m_matchInfo[n].end() && !m->second.file_name.IsEmpty()) {
            LEditor *editor = Frame::Get()->GetMainBook()->OpenFile(m->second.file_name, wxEmptyString, m->second.line_number);
            if (editor && m->second.col >= 0 && m->second.match_len >= 0) {
                int offset = editor->PositionFromLine(m->second.line_number) + m->second.col;
                editor->SetSelection(offset, offset + m->second.match_len);
            }
        }
	}
    
    m_sci->SetCurrentPos(pos);
    m_sci->SetSelectionStart(pos);
    m_sci->SetSelectionEnd(pos);
}

void FindResultsTab::OnMarginClick(wxScintillaEvent& event)
{
	if (event.GetMargin() == 4) {
        m_sci->ToggleFold(m_sci->LineFromPosition(event.GetPosition()));
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

