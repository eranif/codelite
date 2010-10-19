//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : outputtabwindow.cpp
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
#include "macros.h"
#include "globals.h"
#include "manager.h"
#include "outputtabwindow.h"
#include "output_pane.h"
#include "pluginmanager.h"
#include "quickfindbar.h"

BEGIN_EVENT_TABLE(OutputTabWindow, wxPanel)
	EVT_MENU(XRCID("scroll_on_output"),      OutputTabWindow::OnOutputScrolls)
	EVT_MENU(XRCID("search_output"),         OutputTabWindow::OnSearchOutput)
	EVT_MENU(XRCID("clear_all_output"),      OutputTabWindow::OnClearAll)
	EVT_MENU(XRCID("word_wrap_output"),      OutputTabWindow::OnWordWrap)
	EVT_MENU(XRCID("collapse_all"),          OutputTabWindow::OnCollapseAll)
	EVT_MENU(XRCID("repeat_output"),         OutputTabWindow::OnRepeatOutput)
	EVT_UPDATE_UI(XRCID("scroll_on_output"), OutputTabWindow::OnOutputScrollsUI)
	EVT_UPDATE_UI(XRCID("search_output"),    OutputTabWindow::OnSearchOutputUI)
	EVT_UPDATE_UI(XRCID("clear_all_output"), OutputTabWindow::OnClearAllUI)
	EVT_UPDATE_UI(XRCID("word_wrap_output"), OutputTabWindow::OnWordWrapUI)
	EVT_UPDATE_UI(XRCID("collapse_all"),     OutputTabWindow::OnCollapseAllUI)
	EVT_UPDATE_UI(XRCID("repeat_output"),    OutputTabWindow::OnRepeatOutputUI)
	EVT_SCI_DOUBLECLICK(wxID_ANY,            OutputTabWindow::OnMouseDClick)
	EVT_SCI_HOTSPOT_CLICK(wxID_ANY,          OutputTabWindow::OnHotspotClicked)
	EVT_SCI_MARGINCLICK(wxID_ANY,            OutputTabWindow::OnMarginClick)
END_EVENT_TABLE()


OutputTabWindow::OutputTabWindow(wxWindow *parent, wxWindowID id, const wxString &name)
		: wxPanel(parent, id)
		, m_name(name)
		, m_tb(NULL)
		, m_sci(NULL)
		, m_outputScrolls(true)
		, m_autoAppear(true)
		, m_autoAppearErrors(false)
		, m_errorsFirstLine(false)
		, m_findBar(NULL)
{
	CreateGUIControls();
	wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit),   NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit),   NULL, this);
	wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
}

OutputTabWindow::~OutputTabWindow()
{
	wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit),   NULL, this);
	wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(OutputTabWindow::OnEdit),   NULL, this);
	wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);
	wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(OutputTabWindow::OnEditUI), NULL, this);}

void OutputTabWindow::DefineMarker(wxScintilla *sci, int marker, int markerType, wxColor fore, wxColor back)
{
	sci->MarkerDefine(marker, markerType);
	sci->MarkerSetForeground(marker, fore);
	sci->MarkerSetBackground(marker, back);
}

void OutputTabWindow::InitStyle(wxScintilla *sci, int lexer, bool folding)
{
	sci->SetLexer(lexer);
	sci->StyleClearAll();

	for(int i=0; i<=wxSCI_STYLE_DEFAULT; i++) {
		sci->StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		sci->StyleSetForeground(i, DrawingUtils::GetTextCtrlTextColour());
	}

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	sci->StyleSetFont(0, font);

	sci->SetIndicatorCurrent(1);
#ifdef __WXMSW__
	int facttor = 2;
#else
	int facttor = 5;
#endif
	sci->IndicatorSetForeground(1, MakeColourLighter(wxT("GOLD"), facttor));
	sci->IndicatorSetForeground(2, MakeColourLighter(wxT("RED"), 4));
	sci->IndicatorSetStyle(1, wxSCI_INDIC_ROUNDBOX);
	sci->IndicatorSetStyle(2, wxSCI_INDIC_ROUNDBOX);
	sci->IndicatorSetUnder(1, true);
	sci->IndicatorSetUnder(2, true);

	sci->IndicatorSetAlpha(1, 70);
	sci->IndicatorSetAlpha(2, 70);
	
	sci->SetHotspotActiveUnderline (true);
	sci->SetHotspotActiveForeground(true, wxT("BLUE"));
	sci->SetHotspotSingleLine(true);
	sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	sci->SetMarginMask(4, wxSCI_MASK_FOLDERS);

	sci->SetMarginWidth(0, 0);
	sci->SetMarginWidth(1, 0);
	sci->SetMarginWidth(2, 0);

	if (folding) {
		sci->SetMarginWidth(4, 16);
		sci->SetProperty(wxT("fold"), wxT("1"));
		sci->SetMarginSensitive(4, true);
	}

	// current line marker
	DefineMarker(sci, 0x7, wxSCI_MARK_ARROW, wxColor(0x00, 0x00, 0x00), wxColor(0xff, 0xc8, 0xc8));

	wxColor fore(0xff, 0xff, 0xff);
	wxColor back(0x80, 0x80, 0x80);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_ARROWDOWN,  fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_ARROW,      fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_ARROW,      fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN,  fore, back);
	DefineMarker(sci, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, fore, back);

	sci->SetWrapStartIndent(4);
	sci->SetWrapVisualFlags(2);

	sci->SetScrollWidthTracking(true);

	sci->SetReadOnly(true);
}

void OutputTabWindow::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	m_hSizer = new wxBoxSizer(wxHORIZONTAL);


	// Create the default scintilla control
	m_sci = new wxScintilla(this);
	InitStyle(m_sci, wxSCI_LEX_CONTAINER, false);

	// Add the find bar
	m_findBar = new QuickFindBar(this);
	m_findBar->Connect(m_findBar->GetCloseButtonId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(OutputTabWindow::OnHideSearchBar), NULL, this);
	m_findBar->SetEditor(m_sci);

	mainSizer->Add(m_findBar, 0, wxEXPAND);
	mainSizer->Add(m_hSizer, 1, wxEXPAND|wxALL, 0);

	BitmapLoader *bmpLoader = PluginManager::Get()->GetStdIcons();

	//Create the toolbar
	m_tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL|wxTB_NODIVIDER);

	m_tb->AddTool(XRCID("scroll_on_output"),
	              wxT("Scroll on Output"),
	              wxXmlResource::Get()->LoadBitmap(wxT("link_editor")),
	              wxT("Scroll on Output"),
	              wxITEM_CHECK);
	m_tb->ToggleTool(XRCID("scroll_on_output"), m_outputScrolls);

	m_tb->AddTool(XRCID("word_wrap_output"),
	              wxT("Word Wrap"),
	              wxXmlResource::Get()->LoadBitmap(wxT("word_wrap")),
	              wxT("Word Wrap"),
	              wxITEM_CHECK);

	m_tb->AddTool(XRCID("clear_all_output"),
	              wxT("Clear All"),
	              bmpLoader->LoadBitmap(wxT("output-pane/16/clear")),
	              wxT("Clear All"));

	m_tb->AddTool(XRCID("search_output"),
	              wxT("Find..."),
	              bmpLoader->LoadBitmap(wxT("toolbars/16/search/find")),
	              wxT("Find..."),
				  wxITEM_CHECK);

	m_tb->AddTool(XRCID("collapse_all"), _("Fold All Results"),
	              wxXmlResource::Get()->LoadBitmap(wxT("fold_airplane")),
	              _("Fold All Results"));

	m_tb->AddTool(XRCID("repeat_output"), _("Repeat"),
	               bmpLoader->LoadBitmap(wxT("output-pane/16/reload")),
	              _("Repeat"));

	m_tb->Realize();

#ifdef __WXMAC__
	m_hSizer->Add(m_sci, 1, wxEXPAND);
	m_hSizer->Add(m_tb, 0, wxEXPAND);
#else
	m_hSizer->Add(m_tb, 0, wxEXPAND);
	m_hSizer->Add(m_sci, 1, wxEXPAND);
#endif

	// Hide the find bar by default
	m_findBar->Hide();
	m_hSizer->Layout();
}

void OutputTabWindow::Clear()
{
	if (m_sci) {
		m_sci->SetReadOnly(false);
		m_sci->ClearAll();
		m_sci->SetScrollWidth(1);
		m_sci->SetReadOnly(true);
	}
}

void OutputTabWindow::AppendText(const wxString &text)
{
	if (m_sci) {
		if (m_autoAppear && m_sci->GetLength() == 0) {
			ManagerST::Get()->ShowOutputPane(m_name);
		}

		//----------------------------------------------
		// enable writing
		m_sci->SetReadOnly(false);

		if (m_outputScrolls) {
			// the next 4 lines make sure that the caret is at last line
			// and is visible
			m_sci->SetSelectionEnd(m_sci->GetLength());
			m_sci->SetSelectionStart(m_sci->GetLength());
			m_sci->SetCurrentPos(m_sci->GetLength());
			m_sci->EnsureCaretVisible();
		}

		// add the text
		m_sci->InsertText(m_sci->GetLength(), text );

		if (m_outputScrolls) {
			// the next 4 lines make sure that the caret is at last line
			// and is visible
			m_sci->SetSelectionEnd(m_sci->GetLength());
			m_sci->SetSelectionStart(m_sci->GetLength());
			m_sci->SetCurrentPos(m_sci->GetLength());
			m_sci->EnsureCaretVisible();
		}

		// enable readonly mode
		m_sci->SetReadOnly(true);

	}
}

void OutputTabWindow::OnOutputScrolls(wxCommandEvent &e)
{
	m_outputScrolls = !m_outputScrolls;
}

void OutputTabWindow::OnOutputScrollsUI(wxUpdateUIEvent& e)
{
	e.Enable(true);
}

void OutputTabWindow::OnClearAll(wxCommandEvent &e)
{
	Clear();
}

void OutputTabWindow::OnClearAllUI(wxUpdateUIEvent& e)
{
	e.Enable(m_sci && m_sci->GetLength() > 0);
}

void OutputTabWindow::OnWordWrap(wxCommandEvent &e)
{
	if(m_sci) {
	m_sci->SetWrapMode(m_sci->GetWrapMode() == wxSCI_WRAP_WORD ? wxSCI_WRAP_NONE : wxSCI_WRAP_WORD);
	}
}

void OutputTabWindow::OnWordWrapUI(wxUpdateUIEvent& e)
{
	e.Enable(true);
}

void OutputTabWindow::OnCollapseAll(wxCommandEvent& e)
{
	if(m_sci) {
	int maxLine = m_sci->GetLineCount();
	// do two passes: first see if any folds can be collapsed
	// if not, then expand instead
	bool done = false;
	for (int pass = 0; pass < 2 && !done; pass++) {
		for (int line = 0; line < maxLine; line++) {
			if ((m_sci->GetFoldLevel(line) & wxSCI_FOLDLEVELHEADERFLAG) &&
			        m_sci->GetFoldExpanded(line) == !pass) {
				m_sci->ToggleFold(line);
				done = true;
			}
		}
	}
	}
}

void OutputTabWindow::OnCollapseAllUI(wxUpdateUIEvent& e)
{
	e.Enable(m_sci && m_sci->GetLength() > 0);
}

void OutputTabWindow::OnRepeatOutput(wxCommandEvent& e)
{
	e.Skip();
}

void OutputTabWindow::OnRepeatOutputUI(wxUpdateUIEvent& e)
{
	e.Enable(false);
}

void OutputTabWindow::OnMouseDClick(wxScintillaEvent& e)
{
	e.Skip();
}

void OutputTabWindow::OnHotspotClicked(wxScintillaEvent& e)
{
	OnMouseDClick(e);
}

void OutputTabWindow::OnMarginClick(wxScintillaEvent& e)
{
	if (m_sci && e.GetMargin() == 4) {
		m_sci->ToggleFold(m_sci->LineFromPosition(e.GetPosition()));
	}
}

bool OutputTabWindow::IsFocused()
{
	wxWindow *win = wxWindow::FindFocus();
	return (win && win == m_sci);
}

void OutputTabWindow::OnEditUI(wxUpdateUIEvent& e)
{
	if ( !IsFocused() || !m_sci ) {
		e.Skip();
		return;
	}

	switch (e.GetId()) {
	case wxID_COPY:
		e.Enable( m_sci->GetSelectedText().IsEmpty() == false );
		break;
	case wxID_SELECTALL:
		e.Enable(true);
		break;
	default:
		break;
	}
}

void OutputTabWindow::OnEdit(wxCommandEvent &e)
{
	if ( !IsFocused() || !m_sci ) {
		e.Skip();
		return;
	}

	switch (e.GetId()) {
	case wxID_COPY:
		m_sci->Copy();
		break;
	case wxID_SELECTALL:
		m_sci->SelectAll();
		break;
	default:
		break;
	}
}

void OutputTabWindow::OnSearchOutput(wxCommandEvent& e)
{
	if(m_findBar->IsShown()) {
		m_findBar->Hide();

	} else {
		m_findBar->Show();

	}
	GetSizer()->Layout();
}

void OutputTabWindow::OnSearchOutputUI(wxUpdateUIEvent& e)
{
	e.Enable(m_sci && m_sci->GetLength() > 0);
	e.Check (m_findBar->IsShown());
}

void OutputTabWindow::OnHideSearchBar(wxCommandEvent& e)
{
	m_findBar->Hide();
	GetSizer()->Layout();
}
