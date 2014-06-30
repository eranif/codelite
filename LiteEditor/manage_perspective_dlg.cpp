//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : manage_perspective_dlg.cpp
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

#include "manage_perspective_dlg.h"
#include "windowattrmanager.h"
#include "manager.h"
#include "perspectivemanager.h"

ManagePerspectivesDlg::ManagePerspectivesDlg( wxWindow* parent )
	: ManagePerspectivesBaseDlg( parent )
{
	m_listBox->Clear();
	m_listBox->Append(ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives());
	GetSizer()->Fit(this);
	WindowAttrManager::Load(this, wxT("ManagePerspectivesDlg"), NULL);
}

ManagePerspectivesDlg::~ManagePerspectivesDlg()
{
	WindowAttrManager::Save(this, wxT("ManagePerspectivesDlg"), NULL);
}

void ManagePerspectivesDlg::OnDeletePerspective( wxCommandEvent& event )
{
	wxString sel = m_listBox->GetStringSelection();
	if(sel.IsEmpty())
		return;

	if(::wxMessageBox(wxString::Format(_("Are you sure you want to delete perspective '%s'?"), sel.c_str()), wxT("CodeLite"), wxYES_NO|wxICON_QUESTION) == wxYES) {
		ManagerST::Get()->GetPerspectiveManager().Delete(sel);
		DoPopulateList();
	}
}

void ManagePerspectivesDlg::OnDeletePerspectiveUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}

void ManagePerspectivesDlg::OnRenamePerspective( wxCommandEvent& event )
{
	wxString sel = m_listBox->GetStringSelection();
	if(sel.IsEmpty())
		return;

	wxString newName = ::wxGetTextFromUser(_("Enter new name:"), _("Rename perspective"));
	if(newName.IsEmpty())
		return;

	ManagerST::Get()->GetPerspectiveManager().Rename(sel, newName);
	DoPopulateList();
}

void ManagePerspectivesDlg::OnRenamePerspectiveUI( wxUpdateUIEvent& event )
{
	event.Enable(m_listBox->GetSelection() != wxNOT_FOUND);
}

void ManagePerspectivesDlg::DoPopulateList()
{
	// Refresh the list
	m_listBox->Clear();
	m_listBox->Append(ManagerST::Get()->GetPerspectiveManager().GetAllPerspectives());
}

