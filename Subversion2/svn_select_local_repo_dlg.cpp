//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : svn_select_local_repo_dlg.cpp
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

#include "svn_select_local_repo_dlg.h"

#include "subversion2.h"
#include "windowattrmanager.h"

#include <wx/menu.h>

SvnSelectLocalRepoDlg::SvnSelectLocalRepoDlg( wxWindow* parent, Subversion2 *plugin, const wxString &curpath )
	: SvnSelectLocalRepoBase( parent )
	, m_svn(plugin)
{
	m_listBoxPaths->Clear();
	m_listBoxPaths->Append(m_svn->GetSettings().GetRepos());
	m_dirPicker1->SetPath(curpath);
    SetName("SvnSelectLocalRepoDlg");
	WindowAttrManager::Load(this);
}

SvnSelectLocalRepoDlg::~SvnSelectLocalRepoDlg()
{
	
}

void SvnSelectLocalRepoDlg::OnPathSelected( wxCommandEvent& event )
{
	wxArrayInt selections;
	m_listBoxPaths->GetSelections(selections);
	if(selections.GetCount() == 1) {
		m_dirPicker1->SetPath(m_listBoxPaths->GetString(selections.Item(0)));
	}
}

void SvnSelectLocalRepoDlg::OnPathActivated( wxCommandEvent& event )
{
	wxArrayInt selections;
	m_listBoxPaths->GetSelections(selections);
	if(!selections.IsEmpty()) {
		m_dirPicker1->SetPath(m_listBoxPaths->GetString(selections.Item(0)));
		EndModal(wxID_OK);
	}
}

void SvnSelectLocalRepoDlg::OnMenu( wxMouseEvent& event )
{
	wxArrayInt selections;
	m_listBoxPaths->GetSelections(selections);
	if(!selections.IsEmpty()) {
		wxMenu menu;
		if(selections.GetCount() == 1) {
			menu.Append(wxID_DELETE, _("Remove path"), _("Remove path"), wxITEM_NORMAL);
			
		} else {
			menu.Append(wxID_DELETE, _("Remove paths"), _("Remove paths"), wxITEM_NORMAL);
		}

		menu.Connect(wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SvnSelectLocalRepoDlg::OnRemoveEntry), NULL, this);
		m_listBoxPaths->PopupMenu(&menu);
	}
}

wxString SvnSelectLocalRepoDlg::GetPath() const
{
	return m_dirPicker1->GetPath();
}


void SvnSelectLocalRepoDlg::OnRemoveEntry(wxCommandEvent& e)
{
	wxArrayInt ids;
	m_listBoxPaths->GetSelections(ids);

	if(ids.IsEmpty())
		return;

	SvnSettingsData ssd = m_svn->GetSettings();
	const wxArrayString &repos = ssd.GetRepos();
	wxArrayString modRepos = repos;

	for(size_t i=0; i<ids.GetCount(); i++) {
		int id = ids.Item(i);
		wxString str = m_listBoxPaths->GetString(id);
		if(str.IsEmpty() == false && str != _("<No repository path is selected>")) {
			int where = modRepos.Index(str);
			if(where != wxNOT_FOUND) {
				modRepos.RemoveAt(where);
			}
		}
	}
	
	ssd.SetRepos(modRepos);
	m_svn->SetSettings(ssd);
	
	m_listBoxPaths->Clear();
	m_listBoxPaths->Append(m_svn->GetSettings().GetRepos());
}
