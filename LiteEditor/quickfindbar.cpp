//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickfindbar.cpp
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
#include "manager.h"
#include <wx/textctrl.h>
#include <wx/wxscintilla.h>
#include "stringsearcher.h"
#include "quickfindbar.h"

#define CONTROL_ALIGN_STYLE wxLEFT|wxRIGHT|wxALIGN_CENTER_VERTICAL

BEGIN_EVENT_TABLE(QuickFindBar, wxPanel)
	EVT_BUTTON(XRCID("close_quickfind"), QuickFindBar::OnHide)
	EVT_BUTTON(XRCID("find_next_quick"), QuickFindBar::OnNext)
	EVT_BUTTON(XRCID("find_prev_quick"), QuickFindBar::OnPrev)

	EVT_TEXT(XRCID("find_what_quick"),   QuickFindBar::OnText)

	EVT_CHECKBOX(wxID_ANY, QuickFindBar::OnCheckBox)

	EVT_UPDATE_UI(XRCID("find_next_quick"), QuickFindBar::OnUpdateUI)
	EVT_UPDATE_UI(XRCID("find_prev_quick"), QuickFindBar::OnUpdateUI)
END_EVENT_TABLE()


QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id)
		: wxPanel(parent, id)
		, m_findWhat(NULL)
		, m_sci(NULL)
		, m_flags(0)
{
	Hide();
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);

	wxButton *btn(NULL);
	m_closeButton = new wxBitmapButton(this, XRCID("close_quickfind"), wxXmlResource::Get()->LoadBitmap(wxT("page_close16")));
	mainSizer->Add(m_closeButton, 0, CONTROL_ALIGN_STYLE, 2);
	m_closeButton->SetToolTip(wxT("Close QuickFind Bar"));

	wxStaticText *text = new wxStaticText(this, wxID_ANY, wxT("Find:"));
	mainSizer->Add(text, 0, CONTROL_ALIGN_STYLE, 2);

	m_findWhat = new wxTextCtrl(this, XRCID("find_what_quick"), wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_findWhat->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	m_findWhat->SetMinSize(wxSize(200,-1));
	m_findWhat->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(QuickFindBar::OnKeyDown), NULL, this);
	m_findWhat->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(QuickFindBar::OnEnter), NULL, this);

	mainSizer->Add(m_findWhat, 1, CONTROL_ALIGN_STYLE, 5);

	btn = new wxButton(this, XRCID("find_next_quick"), wxT("Next"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	mainSizer->Add(btn, 0, CONTROL_ALIGN_STYLE, 5);
	btn->SetDefault();

	btn = new wxButton(this, XRCID("find_prev_quick"), wxT("Prev"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	mainSizer->Add(btn, 0, CONTROL_ALIGN_STYLE, 5);

	wxStaticBoxSizer *staticSizer = new wxStaticBoxSizer(wxHORIZONTAL, this);
	mainSizer->Add(staticSizer, 0, CONTROL_ALIGN_STYLE, 5);

	wxCheckBox *check = new wxCheckBox(this, XRCID("match_case_quick"), wxT("Case"));
	staticSizer->Add(check, 0, wxEXPAND|wxALL, 5);

	check = new wxCheckBox(this, XRCID("match_word_quick"), wxT("Word"));
	staticSizer->Add(check, 0, wxEXPAND|wxALL, 5);

	check = new wxCheckBox(this, XRCID("match_regexp_quick"), wxT("Regexp"));
	staticSizer->Add(check, 0, wxEXPAND|wxALL, 5);

	wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnCopy),      NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnPaste),     NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnSelectAll), NULL, this);

	mainSizer->Layout();

}

bool QuickFindBar::Show(bool show)
{
	bool res = wxPanel::Show(show);
	if (res) {
		GetParent()->GetSizer()->Show(this, show, true);
		GetParent()->GetSizer()->Layout();
	}
	if (!m_sci) {
		// nothing to do
	} else if (!show) {
		m_sci->SetFocus();
	} else {
		wxString findWhat = m_sci->GetSelectedText().BeforeFirst(wxT('\n'));
		if (!findWhat.IsEmpty()) {
			m_findWhat->SetValue(m_sci->GetSelectedText().BeforeFirst(wxT('\n')));
		}
		m_findWhat->SelectAll();
		m_findWhat->SetFocus();
	}
	return res;
}

void QuickFindBar::DoSearch(bool fwd, bool incr)
{
	if (!m_sci || m_sci->GetLength() == 0 || m_findWhat->GetValue().IsEmpty())
		return;

	wxString find = m_findWhat->GetValue();
	wxString text = m_sci->GetText();

	int start = -1, stop = -1;
	m_sci->GetSelection(&start, &stop);

	int offset = !fwd || incr ? start : stop;
	int flags = m_flags | (fwd ? 0 : wxSD_SEARCH_BACKWARD);
	int pos = 0, len = 0;

	if (!StringFindReplacer::Search(text, offset, find, flags, pos, len)) {
		// wrap around and try again
		offset = fwd ? 0 : text.Len()-1;
		if (!StringFindReplacer::Search(text, offset, find, flags, pos, len)) {
			m_findWhat->SetBackgroundColour(wxT("PINK"));
			return;
		}
	}
	m_findWhat->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
	m_sci->SetSelection(pos, pos+len);
}

void QuickFindBar::OnHide(wxCommandEvent &e)
{
	wxUnusedVar(e);
	Show(false);
}

void QuickFindBar::OnNext(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoSearch(true, false);
}

void QuickFindBar::OnPrev(wxCommandEvent &e)
{
	wxUnusedVar(e);
	DoSearch(false, false);
}

void QuickFindBar::OnText(wxCommandEvent& e)
{
	wxUnusedVar(e);
	DoSearch(true, true);
}

void QuickFindBar::OnKeyDown(wxKeyEvent& e)
{
	switch (e.GetKeyCode()) {
	case WXK_ESCAPE: {
		wxCommandEvent cmd(wxEVT_COMMAND_BUTTON_CLICKED, m_closeButton->GetId());
		cmd.SetEventObject(m_closeButton);
		GetEventHandler()->AddPendingEvent(cmd);
		break;
	}
	default:
		e.Skip();
	}
}

void QuickFindBar::OnCheckBox(wxCommandEvent &e)
{
	size_t flag = 0;
	if (e.GetId() == XRCID("match_case_quick")) {
		flag = wxSD_MATCHCASE;
	} else if (e.GetId() == XRCID("match_word_quick")) {
		flag = wxSD_MATCHWHOLEWORD;
	} else if (e.GetId() == XRCID("match_regexp_quick")) {
		flag = wxSD_REGULAREXPRESSION;
	}
	if (e.IsChecked()) {
		m_flags |= flag;
	} else {
		m_flags &= ~flag;
	}
}

void QuickFindBar::OnUpdateUI(wxUpdateUIEvent &e)
{
	e.Enable(ManagerST::Get()->IsShutdownInProgress() == false && m_sci && m_sci->GetLength() > 0 && !m_findWhat->GetValue().IsEmpty());
}

void QuickFindBar::OnEnter(wxCommandEvent& e)
{
	wxUnusedVar(e);
	bool alt = wxGetKeyState(WXK_ALT);
	wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, alt ? XRCID("find_prev_quick") : XRCID("find_next_quick"));
	GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::OnCopy(wxCommandEvent& e)
{
	if ( IsFocused() && m_findWhat->CanCopy() ) {
		m_findWhat->Copy();
	} else {
		e.Skip();
	}
}

void QuickFindBar::OnPaste(wxCommandEvent& e)
{
	if ( IsFocused() && m_findWhat->CanPaste() ) {
		m_findWhat->Paste();
	} else {
		e.Skip();
	}
}

void QuickFindBar::OnSelectAll(wxCommandEvent& e)
{
	if ( IsFocused() ) {
		m_findWhat->SelectAll();
	} else {
		e.Skip();
	}
}

bool QuickFindBar::IsFocused()
{
	wxWindow *win = wxWindow::FindFocus();
	return win && win == m_findWhat;
}
