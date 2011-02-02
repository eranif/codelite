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
#include "globals.h"
#include <wx/app.h>
#include <wx/log.h>
#include "cscopetab.h"
#include "drawingutils.h"
#include "cscopedbbuilderthread.h"
#include "imanager.h"
#include "fileextmanager.h"
#include "workspace.h"
#include "bitmap_loader.h"
#include <wx/treectrl.h>

CscopeTab::CscopeTab( wxWindow* parent, IManager *mgr )
		: CscopeTabBase( parent )
		, m_table(NULL)
		, m_mgr(mgr)
{
	CScopeConfData data;
	MSWSetNativeTheme(m_treeCtrlResults);
	
	m_mgr->GetConfigTool()->ReadObject(wxT("CscopeSettings"), &data);

	const wxString SearchScope[] = { wxTRANSLATE("Entire Workspace"), wxTRANSLATE("Active Project") };
	m_stringManager.AddStrings(sizeof(SearchScope)/sizeof(wxString), SearchScope, data.GetScanScope(), m_choiceSearchScope);
	
	m_treeCtrlResults->AddColumn(_("Scope"),   300);
	m_treeCtrlResults->AddColumn(_("Line"),    50);
	m_treeCtrlResults->AddColumn(_("Pattern"), 1000);
	m_treeCtrlResults->AddRoot(_("CScope"));
	
	wxImageList *imageList = new wxImageList(16, 16, true);
	imageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("mime/16/c")));                              // 0
	imageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("mime/16/cpp")));                            // 1
	imageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("mime/16/h")));                              // 2
	imageList->Add(m_mgr->GetStdIcons()->LoadBitmap(wxT("mime/16/text")));                           // 3
	m_treeCtrlResults->AssignImageList( imageList );
	
	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	m_font = wxFont( defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL );
	
	m_checkBoxUpdateDb->SetValue(data.GetRebuildOption());
	m_checkBoxRevertedIndex->SetValue(data.GetBuildRevertedIndexOption());
	SetMessage(_("Ready"), 0);
	
	m_treeCtrlResults->Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(CscopeTab::OnItemActivated), NULL, this);
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
	m_treeCtrlResults->DeleteChildren(m_treeCtrlResults->GetRootItem());
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
	m_treeCtrlResults->DeleteChildren(m_treeCtrlResults->GetRootItem());

	//add hidden root
	wxTreeItemId root = m_treeCtrlResults->GetRootItem();

	CscopeResultTable::iterator iter = m_table->begin();
	for (; iter != m_table->end(); iter++ ) {
		wxString file = iter->first;

		//add item for this file
		wxTreeItemId parent;
		std::set<wxString> insertedItems;
		
		std::vector< CscopeEntryData >* vec = iter->second;
		std::vector< CscopeEntryData >::iterator it = vec->begin();
		for ( ; it != vec->end(); it++ ) {
			CscopeEntryData entry = *it;
			if (parent.IsOk() == false) {
				//add parent item
				CscopeEntryData parent_entry = entry;
				parent_entry.SetKind(KindFileNode);
				
				// detemine the image ID
				int imgId(3); // text
				switch(FileExtManager::GetType(entry.GetFile())) {
				case FileExtManager::TypeHeader:
					imgId = 2;
					break;
				
				case FileExtManager::TypeSourceC:
					imgId = 0;
					break;
					
				case FileExtManager::TypeSourceCpp:
					imgId = 1;
					break;
				
				default:
					imgId = 3;
					break;
				}
				parent = m_treeCtrlResults->AppendItem(root, entry.GetFile(), imgId, imgId, NULL);
				m_treeCtrlResults->SetItemFont(parent, m_font);
				
				wxColour rootItemColour = DrawingUtils::LightColour(wxT("LIGHT GRAY"), 3.0);
				m_treeCtrlResults->SetItemBackgroundColour(parent, rootItemColour);
			}
			
			// Dont insert duplicate entries to the match view
			wxString display_string;
			display_string << _("Line: ") << entry.GetLine() << wxT(", ") << entry.GetScope() << wxT(", ") << entry.GetPattern();
			if(insertedItems.find(display_string) == insertedItems.end()) {
				insertedItems.insert(display_string);
				wxTreeItemId item = m_treeCtrlResults->AppendItem(parent, entry.GetScope(), wxNOT_FOUND, wxNOT_FOUND, new CscopeTabClientData(entry));
				m_treeCtrlResults->SetItemFont(item, m_font);
				m_treeCtrlResults->SetItemText(item, 1, wxString::Format(wxT("%d"), entry.GetLine()));
				m_treeCtrlResults->SetItemText(item, 2, entry.GetPattern());
			}
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

				if(m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, data->GetEntry().GetLine()-1)) {
					IEditor *editor = m_mgr->GetActiveEditor();
					if( editor && editor->GetFileName().GetFullPath() == fn.GetFullPath() && !GetFindWhat().IsEmpty()) {
						// Find and select the entry in the file
						editor->FindAndSelect(data->GetEntry().GetPattern(), GetFindWhat(), editor->GetCurrentPosition(), m_mgr->GetNavigationMgr());
					}
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
	SetMessage(_("Ready"), 0);
	Clear();
}

void CscopeTab::OnClearResultsUI(wxUpdateUIEvent& e)
{
	e.Enable(m_treeCtrlResults->HasChildren( m_treeCtrlResults->GetRootItem()));
}

void CscopeTab::OnChangeSearchScope(wxCommandEvent& e)
{
	CScopeConfData data;
	data.SetScanScope(m_stringManager.GetStringSelection());
	data.SetRebuildDbOption(m_checkBoxUpdateDb->IsChecked());
	data.SetBuildRevertedIndexOption(m_checkBoxRevertedIndex->IsChecked());
	m_mgr->GetConfigTool()->WriteObject(wxT("CscopeSettings"), &data);
}
