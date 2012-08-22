//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : navbar.cpp
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
#include <vector>
#include "editor_config.h"
#include "ctags_manager.h"
#include "cl_editor.h"
#include "manager.h"
#include "frame.h"
#include "navbar.h"

NavBar::NavBar(wxWindow* parent)
	: NavBarControlBaseClass(parent)
{
#ifdef __WXMAC__
	m_scope->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
	m_func->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    long sashPos = -1;
    EditorConfigST::Get()->GetLongValue(wxT("NavBarSashPos"), sashPos);
    
    if ( sashPos != wxNOT_FOUND ) {
        m_splitter->SetSashPosition(sashPos);
    }
}

NavBar::~NavBar()
{
    EditorConfigST::Get()->SaveLongValue(wxT("NavBarSashPos"), m_splitter->GetSashPosition());
}

void NavBar::OnScopeListMouseDown(wxMouseEvent& e)
{
	if (!ManagerST::Get()->IsWorkspaceOpen())
		return;

	LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if (!editor)
		return;

	std::vector<wxString> scopes;
	TagsManagerST::Get()->GetScopesFromFile(editor->GetFileName(), scopes);

	m_scope->Freeze();

	wxString cursel = m_scope->GetStringSelection();
	m_scope->Clear();
	for (unsigned i = 0; i < scopes.size(); i++) {
		m_scope->AppendString(scopes[i]);
	}
	if (!cursel.IsEmpty()) {
		m_scope->SetStringSelection(cursel);
	}

	m_scope->Thaw();

	e.Skip();
}

void NavBar::OnScope(wxCommandEvent& e)
{
	size_t sel = e.GetSelection();
	if (sel < m_scope->GetCount()) {
		m_tags.clear();
		m_func->Clear();
	}
}

void NavBar::OnFuncListMouseDown(wxMouseEvent& e)
{
	if (!ManagerST::Get()->IsWorkspaceOpen())
		return;

	LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if (!editor)
		return;

	wxString scope = m_scope->GetStringSelection();
	if (scope.IsEmpty())
		return;

	m_tags.clear();
	TagsManagerST::Get()->TagsFromFileAndScope(editor->GetFileName(), scope, m_tags);

	m_func->Freeze();

	wxString cursel = m_func->GetStringSelection();
	m_func->Clear();
	for (size_t i = 0; i < m_tags.size(); i++) {
		m_func->AppendString(m_tags[i]->GetDisplayName());
	}
	if (!cursel.IsEmpty()) {
		m_func->SetStringSelection(cursel);
	}

	m_func->Thaw();

	e.Skip();
}

void NavBar::OnFunction(wxCommandEvent& e)
{
	LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if (!editor)
		return;

	size_t sel = e.GetSelection();
	if (sel >= m_tags.size())
		return;

	wxString pattern = m_tags[sel]->GetPattern();
	wxString name = m_tags[sel]->GetName();
	editor->FindAndSelect(pattern, name);
	editor->SetActive();
}

void NavBar::DoShow(bool s)
{
	if (Show(s)) {
		GetParent()->GetSizer()->Layout();
	}
}

void NavBar::UpdateScope(TagEntryPtr tag)
{
	size_t sel = m_func->GetSelection();
	if (tag && sel < m_tags.size() && *m_tags[sel] == *tag)
		return;

	Freeze();

	m_tags.clear();
	m_scope->Clear();
	m_func->Clear();

	if (tag) {
		m_tags.push_back(tag);
		m_scope->AppendString(tag->GetScope());
		m_func->AppendString(tag->GetDisplayName());
		m_scope->SetSelection(0);
		m_func->SetSelection(0);
	}

	Thaw();
}
