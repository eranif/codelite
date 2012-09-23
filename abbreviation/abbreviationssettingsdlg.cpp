//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : abbreviationssettingsdlg.cpp              
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

#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "abbreviationentry.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include "abbreviationssettingsdlg.h"

AbbreviationsSettingsDlg::AbbreviationsSettingsDlg( wxWindow* parent, IManager *mgr )
		: AbbreviationsSettingsBase( parent )
		, m_data()
		, m_mgr(mgr)
		, m_dirty(false)
		, m_currSelection(wxNOT_FOUND)
{
	WindowAttrManager::Load(this, wxT("AbbreviationsSettingsDlg"), m_mgr->GetConfigTool());
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &m_data);
	DoPopulateItems();
}

AbbreviationsSettingsDlg::~AbbreviationsSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("AbbreviationsSettingsDlg"), m_mgr->GetConfigTool());
}

void AbbreviationsSettingsDlg::OnItemSelected( wxCommandEvent& event )
{
	if(m_dirty) {
		DoSaveCurrent();
	}
	DoSelectItem(event.GetSelection());
}


void AbbreviationsSettingsDlg::OnNew(wxCommandEvent& e)
{
	if(m_dirty) {
		DoSaveCurrent();
	}
	
	wxString name = wxGetTextFromUser(_("What is the name of the new abbreviation name:"), _("New abbreviation..."), wxT(""), this);
	if(name.IsEmpty() == false){
		if( m_listBoxAbbreviations->FindString(name) != wxNOT_FOUND ){
			wxMessageBox(wxString::Format(_("An abbreviation with this name already exists!")));
			return;
		}
		
		int where = m_listBoxAbbreviations->Append(name);
		m_listBoxAbbreviations->SetSelection(where);
		
		m_activeItemName = name;
		m_currSelection = where;
		
		m_textCtrlName->SetValue(name);
		m_textCtrlExpansion->Clear();
		m_textCtrlName->SetFocus();
	}
}

void AbbreviationsSettingsDlg::OnDelete(wxCommandEvent& event)
{
	if(m_activeItemName.IsEmpty() || m_currSelection == wxNOT_FOUND){
		return;
	}
	
	if(wxMessageBox(wxString::Format(_("Are you sure you want to delete '%s'"), m_activeItemName.c_str()),
			_("CodeLite"), wxYES_NO|wxCANCEL|wxCENTER|wxICON_QUESTION, this) != wxYES)
	{
		return;
	}
	
	// delete the entry from the configuration file
	DoDeleteEntry(m_activeItemName);
	
	// delete it
	m_listBoxAbbreviations->Delete((unsigned int) m_currSelection);
	m_textCtrlExpansion->Clear();
	m_textCtrlName->Clear();
	
	// select the previous item in the list
	if(m_listBoxAbbreviations->IsEmpty() == false){
		switch(m_currSelection) {
		case 0:
			m_currSelection = 0;
			m_activeItemName = m_listBoxAbbreviations->GetString(0);
			break;
		default:
			m_currSelection--;
			m_activeItemName = m_listBoxAbbreviations->GetString(m_currSelection);
			break;
		}
	} else {
		m_activeItemName.Empty();
		m_currSelection = wxNOT_FOUND;
	}
	
	if(m_currSelection != wxNOT_FOUND){
		m_listBoxAbbreviations->SetSelection(m_currSelection);
		DoSelectItem(m_currSelection);
	}
}

void AbbreviationsSettingsDlg::OnDeleteUI(wxUpdateUIEvent& event)
{
	event.Enable(m_currSelection != wxNOT_FOUND && m_activeItemName.IsEmpty() == false);
}

void AbbreviationsSettingsDlg::OnMarkDirty(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if(m_activeItemName.IsEmpty() == false) {
		m_dirty = true;
	}
}

void AbbreviationsSettingsDlg::OnSave(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if(m_dirty) {
		DoSaveCurrent();
	}
	m_data.SetAutoInsert(m_checkBoxImmediateInsert->IsChecked());
	m_mgr->GetConfigTool()->WriteObject(wxT("AbbreviationsData"), &m_data);
	EndModal(wxID_OK);
}

void AbbreviationsSettingsDlg::DoPopulateItems()
{
	std::map<wxString, wxString> entries = m_data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.begin();
	for(; iter != entries.end(); iter ++){
		m_listBoxAbbreviations->Append(iter->first);
	}
	
	if(m_listBoxAbbreviations->IsEmpty() == false){
		m_listBoxAbbreviations->Select(0);
		DoSelectItem(0);
	}
	
	m_checkBoxImmediateInsert->SetValue(m_data.GetAutoInsert());
	
	m_dirty = false;	
}

void AbbreviationsSettingsDlg::DoSelectItem(int item)
{
	if(item >= 0){
		wxString name = m_listBoxAbbreviations->GetString(item);
		m_activeItemName = name;
		m_currSelection = item;
		
		m_textCtrlName->SetValue(name);
		
		std::map<wxString, wxString> entries = m_data.GetEntries();
		std::map<wxString, wxString>::iterator iter = entries.find(name);
		if(iter != entries.end()){
			m_textCtrlExpansion->SetValue(iter->second);
		}
		m_dirty = false;
	}
}

void AbbreviationsSettingsDlg::DoSaveCurrent()
{
	if(m_currSelection == wxNOT_FOUND){
		return;
	}
	
	// search for the old item
	std::map<wxString, wxString> entries = m_data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(m_activeItemName);
	if(iter != entries.end()){
		entries.erase( iter );
	}
	
	// insert the new item
	entries[m_textCtrlName->GetValue()] = m_textCtrlExpansion->GetValue();
	m_data.SetEntries(entries);
	
	m_activeItemName = m_textCtrlName->GetValue();
	
	// update the name in the list box
	m_listBoxAbbreviations->SetString((unsigned int) m_currSelection, m_activeItemName);
	m_dirty = false;
	
	m_textCtrlName->SetFocus();
}

void AbbreviationsSettingsDlg::DoDeleteEntry(const wxString &name)
{	
	// search for the old item
	std::map<wxString, wxString> entries = m_data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(name);
	if(iter != entries.end()){
		entries.erase( iter );
	}
	m_data.SetEntries(entries);
}
