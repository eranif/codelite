//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cscopetab.cpp
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
#include "csscopeconfdata.h"
#include "cscopetab.h"
#include "cscopedbbuilderthread.h"
#include "imanager.h"
#include "workspace.h"

CscopeTab::CscopeTab( wxWindow* parent, IManager *mgr )
		: CscopeTabBase( parent )
		, m_table(NULL)
		, m_mgr(mgr)
{
	CSscopeConfData data;
	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &data);
	m_choiceSearchScope->SetStringSelection(data.GetScanScope());
	SetMessage(wxT("Ready"), 0);
}

void CscopeTab::OnItemActivated( wxTreeEvent& event )
{
	wxTreeItemId item = event.GetItem();
	DoItemActivated(item, event);
}

void CscopeTab::Clear()
{
	if (m_table) {
		//free the old table
		FreeTable();
	}
	m_treeCtrlResults->DeleteAllItems();
}

void CscopeTab::BuildTable(CscopeResultTable *table)
{
	if ( !table ) {
		return;
	}

	if (m_table) {
		//free the old table
		FreeTable();
	}

	m_table = table;
	m_treeCtrlResults->DeleteAllItems();

	//add hidden root
	wxTreeItemId root = m_treeCtrlResults->AddRoot(wxT("Root"));

	CscopeResultTable::iterator iter = m_table->begin();
	for (; iter != m_table->end(); iter++ ) {
		wxString file = iter->first;

		//add item for this file
		wxTreeItemId parent;

		std::vector< CscopeEntryData >* vec = iter->second;
		std::vector< CscopeEntryData >::iterator it = vec->begin();
		for ( ; it != vec->end(); it++ ) {
			CscopeEntryData entry = *it;
			if (parent.IsOk() == false) {
				//add parent item
				CscopeEntryData parent_entry = entry;
				parent_entry.SetKind(KindFileNode);
				parent = m_treeCtrlResults->AppendItem(root, entry.GetFile(), wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(parent_entry));
			}

			wxString display_string;
			display_string << wxT("Line: ") << entry.GetLine() << wxT(", ") << entry.GetScope() << wxT(", ") << entry.GetPattern();
			m_treeCtrlResults->AppendItem(parent, display_string, wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(entry));
		}
	}
	FreeTable();
}

void CscopeTab::FreeTable()
{
	if (m_table) {
		CscopeResultTable::iterator iter = m_table->begin();
		for (; iter != m_table->end(); iter++ ) {
			//delete the vector
			delete iter->second;
		}
		m_table->clear();
		delete m_table;
		m_table = NULL;
	}
}
void CscopeTab::OnLeftDClick(wxMouseEvent &event)
{
	// Make sure the double click was done on an actual item
	int flags = wxTREE_HITTEST_ONITEMLABEL;
	wxTreeItemId item = m_treeCtrlResults->GetSelection();
	if ( item.IsOk() ) {
		if ( m_treeCtrlResults->HitTest(event.GetPosition() , flags) == item ) {
			DoItemActivated( item, event );
			return;
		}
	}
	event.Skip();
}

void CscopeTab::DoItemActivated( wxTreeItemId &item, wxEvent &event )
{
	if (item.IsOk()) {
		CscopeTabClientData *data = (CscopeTabClientData*) m_treeCtrlResults->GetItemData(item);
		if (data) {
			wxString wsp_path = m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);

			if (data->GetEntry().GetKind() == KindSingleEntry) {

				//a single entry was activated, open the file
				//convert the file path to absolut path. We do it here, to improve performance
				wxFileName fn(data->GetEntry().GetFile());

				if ( !fn.MakeAbsolute(wsp_path) ) {
					wxLogMessage(wxT("failed to convert file to absolute path"));
				}

				IEditor *currentEditor = m_mgr->GetActiveEditor();
				if(m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, data->GetEntry().GetLine()-1) && currentEditor){
					currentEditor->AddBrowseRecord(m_mgr->GetNavigationMgr());
				}
				return;
			} else if (data->GetEntry().GetKind() == KindFileNode) {
				event.Skip();
				return;
			}
		}
	}
	event.Skip();
}

void CscopeTab::SetMessage(const wxString &msg, int percent)
{
	m_statusMessage->SetLabel(msg);
	m_gauge->SetValue(percent);
}

void CscopeTab::OnClearResults(wxCommandEvent &e)
{
	wxUnusedVar(e);
	SetMessage(wxT("Ready"), 0);
	Clear();
}

void CscopeTab::OnClearResultsUI(wxUpdateUIEvent& e)
{
	e.Enable(m_treeCtrlResults->IsEmpty() == false);
}

void CscopeTab::OnChangeSearchScope(wxCommandEvent& e)
{
	CSscopeConfData data;
	data.SetScanScope(m_choiceSearchScope->GetStringSelection());
	m_mgr->GetConfigTool()->WriteObject(wxT("CscopeSettings"), &data);
}
