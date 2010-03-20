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
#include "editor_config.h"
#include <wx/statline.h>
#include "manager.h"
#include <wx/textctrl.h>
#include <wx/wxscintilla.h>
#include "stringsearcher.h"
#include "quickfindbar.h"

QuickFindBar::QuickFindBar(wxWindow* parent, wxWindowID id)
		: QuickFindBarBase(parent, id)
		, m_sci(NULL)
		, m_flags(0)
{
	Hide();
	m_toolBar1->SetToolBitmapSize(wxSize(16, 16));
	m_toolBar1->FindById(wxID_HIDE)->SetNormalBitmap(wxXmlResource::Get()->LoadBitmap(wxT("pane_close")));
	m_toolBar1->FindById(wxID_SHOW_REPLACE_CONTROLS)->SetNormalBitmap(wxXmlResource::Get()->LoadBitmap(wxT("pencil")));
	m_toolBar1->Realize();

	m_toolBar2->SetToolBitmapSize(wxSize(16, 16));
	m_toolBar2->FindById(wxID_FIND_NEXT)->SetNormalBitmap(wxXmlResource::Get()->LoadBitmap(wxT("next")));
	m_toolBar2->FindById(wxID_FIND_PREVIOUS)->SetNormalBitmap(wxXmlResource::Get()->LoadBitmap(wxT("previous")));
	m_toolBar2->Realize();

	m_toolBarReplace->SetToolBitmapSize(wxSize(16, 16));
	m_toolBarReplace->FindById(wxID_TOOL_REPLACE)->SetNormalBitmap(wxXmlResource::Get()->LoadBitmap(wxT("refresh16")));
	m_toolBarReplace->Realize();
	DoShowControls();

	GetSizer()->Fit(this);
	wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnCopy),      NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnPaste),     NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnSelectAll), NULL, this);

	wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
	wxTheApp->Connect(wxID_PASTE,     wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);
	wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(QuickFindBar::OnEditUI), NULL, this);

	wxTheApp->Connect(XRCID("find_next"),              wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNext),          NULL, this);
	wxTheApp->Connect(XRCID("find_previous"),          wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPrevious),      NULL, this);
	wxTheApp->Connect(XRCID("find_next_at_caret"),     wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindNextCaret),     NULL, this);
	wxTheApp->Connect(XRCID("find_previous_at_caret"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(QuickFindBar::OnFindPreviousCaret), NULL, this);
}

bool QuickFindBar::Show(bool show)
{
	if (!m_sci && show) return false;
	return DoShow(show, wxEmptyString);
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
			m_findWhat->Refresh();
			return;
		}
	}
	m_findWhat->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_findWhat->Refresh();
	m_sci->SetSelection(pos, pos+len);
}

void QuickFindBar::OnHide(wxCommandEvent &e)
{
	Show(false);
	e.Skip();
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
		wxCommandEvent cmd(wxEVT_COMMAND_TOOL_CLICKED, wxID_HIDE);
		cmd.SetEventObject(m_toolBar1);
		m_toolBar1->GetEventHandler()->AddPendingEvent(cmd);
		break;
	}
	default:
		e.Skip();
	}
}

void QuickFindBar::OnUpdateUI(wxUpdateUIEvent &e)
{
	e.Enable(ManagerST::Get()->IsShutdownInProgress() == false && m_sci && m_sci->GetLength() > 0 && !m_findWhat->GetValue().IsEmpty());
}

void QuickFindBar::OnEnter(wxCommandEvent& e)
{
	wxUnusedVar(e);

	bool shift = wxGetKeyState(WXK_SHIFT);
	wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, shift ? wxID_FIND_PREVIOUS : wxID_FIND_NEXT);
	evt.SetEventObject(this);
	GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::OnCopy(wxCommandEvent& e)
{
	wxTextCtrl *ctrl = GetFocusedControl();
	if ( !ctrl ) {
		e.Skip();
		return;
	}

	if (ctrl->CanCopy())
		ctrl->Copy();
}

void QuickFindBar::OnPaste(wxCommandEvent& e)
{
	wxTextCtrl *ctrl = GetFocusedControl();
	if ( !ctrl ) {
		e.Skip();
		return;
	}

	if (ctrl->CanPaste())
		ctrl->Paste();
}

void QuickFindBar::OnSelectAll(wxCommandEvent& e)
{
	wxTextCtrl *ctrl = GetFocusedControl();
	if ( !ctrl ) {
		e.Skip();
	} else {
		ctrl->SelectAll();
	}
}

void QuickFindBar::OnEditUI(wxUpdateUIEvent& e)
{
	wxTextCtrl *ctrl = GetFocusedControl();
	if ( !ctrl ) {
		e.Skip();
		return;
	}

	switch (e.GetId()) {
	case wxID_SELECTALL:
		e.Enable(ctrl->GetValue().IsEmpty() == false);
		break;
	case wxID_COPY:
		e.Enable(ctrl->CanCopy());
		break;
	case wxID_PASTE:
		e.Enable(ctrl->CanPaste());
		break;
	default:
		e.Enable(false);
		break;
	}
}

void QuickFindBar::OnReplace(wxCommandEvent& e)
{
	if (!m_sci)
		return;

	// if there is no selection, invoke search
	wxString selectionText = m_sci->GetSelectedText();
	wxString find          = m_findWhat->GetValue();
	wxString replaceWith   = m_replaceWith->GetValue();

	bool caseSearch        = m_flags & wxSD_MATCHCASE;
	if ( !caseSearch ) {
		selectionText.MakeLower();
		find.MakeLower();
	}

	if (find.IsEmpty())
		return;

	// do we got a match?
	if (selectionText != find)
		DoSearch(true, true);

	else {
		m_sci->ReplaceSelection(replaceWith);
		// and search again
		DoSearch(true, true);
	}
}

void QuickFindBar::OnReplaceUI(wxUpdateUIEvent& e)
{
	e.Enable(   ManagerST::Get()->IsShutdownInProgress() == false &&
	            m_sci                                             &&
	            !m_sci->GetReadOnly()                             &&
	            m_sci->GetLength() > 0                            &&
	            !m_findWhat->GetValue().IsEmpty());
}

wxTextCtrl* QuickFindBar::GetFocusedControl()
{
	wxWindow *win = wxWindow::FindFocus();

	if (win == m_findWhat)
		return m_findWhat;

	else if (win == m_replaceWith)
		return m_replaceWith;

	return NULL;
}

void QuickFindBar::OnReplaceEnter(wxCommandEvent& e)
{
	wxUnusedVar(e);
	wxCommandEvent evt(wxEVT_COMMAND_TOOL_CLICKED, wxID_TOOL_REPLACE);
	GetEventHandler()->AddPendingEvent(evt);
}

void QuickFindBar::ShowReplaceControls(bool show)
{
	if ( show && !m_replaceWith->IsShown()) {
		m_replaceWith->Show();
		m_toolBarReplace->Show();
		m_replaceStaticText->Show();
		GetSizer()->Layout();

	} else if ( !show && m_replaceWith->IsShown()) {
		m_replaceWith->Show(false);
		m_toolBarReplace->Show(false);
		m_replaceStaticText->Show(false);
		GetSizer()->Layout();

	}
}

void QuickFindBar::SetEditor(wxScintilla* sci)
{
	m_sci = sci;
	DoShowControls();
}

void QuickFindBar::OnCheckBoxCase(wxCommandEvent& event)
{
	if (event.IsChecked())
		m_flags |= wxSD_MATCHCASE;
	else
		m_flags &= ~wxSD_MATCHCASE;
}

void QuickFindBar::OnCheckBoxRegex(wxCommandEvent& event)
{
	if (event.IsChecked())
		m_flags |= wxSD_REGULAREXPRESSION;
	else
		m_flags &= ~wxSD_REGULAREXPRESSION;
}

void QuickFindBar::OnCheckBoxWord(wxCommandEvent& event)
{
	if (event.IsChecked())
		m_flags |= wxSD_MATCHWHOLEWORD;
	else
		m_flags &= ~wxSD_MATCHWHOLEWORD;
}

int QuickFindBar::GetCloseButtonId()
{
	return wxID_HIDE;
}

void QuickFindBar::OnToggleReplaceControls(wxCommandEvent& event)
{
	wxUnusedVar(event);
	long v(m_replaceWith->IsShown() ? 0 : 1);
	EditorConfigST::Get()->SaveLongValue(wxT("QuickFindBarShowReplace"), v);
	DoShowControls();
}

void QuickFindBar::DoShowControls()
{
	long v(1);
	EditorConfigST::Get()->GetLongValue(wxT("QuickFindBarShowReplace"), v);
	bool canShowToggleReplaceButton = m_sci && !m_sci->GetReadOnly();
	bool showReplaceControls        = canShowToggleReplaceButton && v;

	ShowReplaceControls(showReplaceControls);
	Refresh();
	GetParent()->GetSizer()->Layout();
}

bool QuickFindBar::Show(const wxString& findWhat)
{
	// Same as Show() but set the 'findWhat' field with findWhat
	return DoShow(true, findWhat);
}

bool QuickFindBar::DoShow(bool s, const wxString& findWhat)
{
	bool res = wxPanel::Show(s);
	if (res) {
		GetParent()->GetSizer()->Layout();
	}

	DoShowControls();

	if (!m_sci) {
		// nothing to do

	} else if (!s) {
		m_sci->SetFocus();

	} else if (findWhat.IsEmpty() == false) {
		m_findWhat->SetValue(findWhat);
		m_findWhat->SelectAll();
		m_findWhat->SetFocus();

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

void QuickFindBar::OnFindNext(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (!m_sci || m_sci->GetLength() == 0)
		return;

	// Highlighted text takes predencese over the current search string
	wxString selectedText = m_sci->GetSelectedText();
	if (selectedText.IsEmpty() == false)
		m_findWhat->SetValue(selectedText);

	DoSearch(true, false);
}

void QuickFindBar::OnFindPrevious(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (!m_sci || m_sci->GetLength() == 0)
		return;

	// Highlighted text takes predencese over the current search string
	wxString selectedText = m_sci->GetSelectedText();
	if (selectedText.IsEmpty() == false)
		m_findWhat->SetValue(selectedText);

	DoSearch(false, false);
}

void QuickFindBar::OnFindNextCaret(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (!m_sci || m_sci->GetLength() == 0)
		return;

	wxString selection( m_sci->GetSelectedText() );
	if (selection.IsEmpty()) {
		// select the word
		long pos   = m_sci->GetCurrentPos();
		long start = m_sci->WordStartPosition(pos, true);
		long end   = m_sci->WordEndPosition(pos, true);

		selection = m_sci->GetTextRange(start, end);
		if (selection.IsEmpty() == false)
			m_sci->SetCurrentPos(start);
	}

	if (selection.IsEmpty())
		return;

	m_findWhat->SetValue(selection);
	DoSearch(true, false);
}

void QuickFindBar::OnFindPreviousCaret(wxCommandEvent& e)
{
	wxUnusedVar(e);
	if (!m_sci || m_sci->GetLength() == 0)
		return;

	wxString selection( m_sci->GetSelectedText() );
	if (selection.IsEmpty()) {
		// select the word
		long pos   = m_sci->GetCurrentPos();
		long start = m_sci->WordStartPosition(pos, true);
		long end   = m_sci->WordEndPosition(pos, true);

		selection = m_sci->GetTextRange(start, end);
		if (selection.IsEmpty() == false)
			m_sci->SetCurrentPos(start);
	}

	if (selection.IsEmpty())
		return;

	m_findWhat->SetValue(selection);
	DoSearch(false, false);
}


void QuickFindBar::OnToggleReplaceControlsUI(wxUpdateUIEvent& event)
{
	if(!m_sci) {
		event.Enable(false);
	} else {
		if(m_sci->GetReadOnly()) {
			event.Enable(false);
		} else if(m_replaceWith->IsShown()) {
			event.Enable(true);
			event.Check(true);
		} else {
			event.Enable(true);
			event.Check(false);
		}
	}
}
