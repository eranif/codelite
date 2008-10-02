#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include "abbreviationentry.h"
#include "imanager.h"
#include "windowattrmanager.h"
#include "abbreviationssettingsdlg.h"

AbbreviationsSettingsDlg::AbbreviationsSettingsDlg( wxWindow* parent, IManager *mgr )
		: AbbreviationsSettingsBase( parent )
		, m_mgr(mgr)
		, m_dirty(false)
		, m_currSelection(wxNOT_FOUND)
{
	WindowAttrManager::Load(this, wxT("AbbreviationsSettingsDlg"), m_mgr->GetConfigTool());
	DoPopulateItems();
}

AbbreviationsSettingsDlg::~AbbreviationsSettingsDlg()
{
	WindowAttrManager::Save(this, wxT("AbbreviationsSettingsDlg"), m_mgr->GetConfigTool());
}

void AbbreviationsSettingsDlg::OnItemSelected( wxCommandEvent& event )
{
	if(m_dirty) {
		if(wxMessageBox(wxT("Would you like to save the current changes?"), wxT("CodeLite"), wxYES_NO|wxCENTER, this) == wxYES){
			DoSaveCurrent();
		}
	}
	DoSelectItem(event.GetSelection());
}

void AbbreviationsSettingsDlg::OnButtonOk( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_OK);
}

void AbbreviationsSettingsDlg::DoPopulateItems()
{
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);
	
	std::map<wxString, wxString> entries = data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.begin();
	for(; iter != entries.end(); iter ++){
		m_listBoxAbbreviations->Append(iter->first);
	}
	
	if(m_listBoxAbbreviations->IsEmpty() == false){
		m_listBoxAbbreviations->Select(0);
		DoSelectItem(0);
	}
	m_dirty = false;
	
}

void AbbreviationsSettingsDlg::DoSelectItem(unsigned int item)
{
	wxString name = m_listBoxAbbreviations->GetString(item);
	m_activeItemName = name;
	m_currSelection = item;
	
	m_textCtrlName->SetValue(name);
	
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);
	
	std::map<wxString, wxString> entries = data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(name);
	if(iter != entries.end()){
		m_textCtrlExpansion->SetValue(iter->second);
	}
	m_dirty = false;
}

void AbbreviationsSettingsDlg::OnDelete(wxCommandEvent& event)
{
	if(m_activeItemName.IsEmpty() || m_currSelection == wxNOT_FOUND){
		return;
	}
	
	if(wxMessageBox(wxString::Format(wxT("Are you sure you want to delete '%s'"), m_activeItemName.c_str()),
			wxT("CodeLite"), wxYES_NO|wxCANCEL|wxCENTER|wxICON_QUESTION, this) != wxYES)
	{
		return;
	}
	
	// delete it
	m_listBoxAbbreviations->Delete((unsigned int) m_currSelection);
	m_textCtrlExpansion->Clear();
	m_textCtrlName->Clear();
	
	// delete the entry from the configuration file
	DoDeleteEntry(m_activeItemName);
	
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

void AbbreviationsSettingsDlg::OnNew(wxCommandEvent& e)
{
	if(m_dirty) {
		if(wxMessageBox(wxT("Would you like to save the current changes?"), wxT("CodeLite"), wxYES_NO|wxCENTER, this) == wxYES){
			DoSaveCurrent();
		}
	}
	
	wxString name = wxGetTextFromUser(wxT("Insert new abbreviation name:"), wxT("New abbreviation..."), wxT(""), this);
	if(name.IsEmpty() == false){
		if( m_listBoxAbbreviations->FindString(name) != wxNOT_FOUND ){
			wxMessageBox(wxString::Format(wxT("An abbreviation with this name already exist!")));
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

void AbbreviationsSettingsDlg::OnSave(wxCommandEvent& event)
{
	wxUnusedVar(event);
	DoSaveCurrent();
}

void AbbreviationsSettingsDlg::OnSaveUI(wxUpdateUIEvent& event)
{
	event.Enable(m_dirty && m_activeItemName.IsEmpty() == false);
}

void AbbreviationsSettingsDlg::OnMarkDirty(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if(m_activeItemName.IsEmpty() == false){
		m_dirty = true;
	}
}

void AbbreviationsSettingsDlg::DoSaveCurrent()
{
	if(m_currSelection == wxNOT_FOUND){
		return;
	}
	
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);
	
	// search for the old item
	std::map<wxString, wxString> entries = data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(m_activeItemName);
	if(iter != entries.end()){
		entries.erase( iter );
	}
	
	// insert the new item
	entries[m_textCtrlName->GetValue()] = m_textCtrlExpansion->GetValue();
	data.SetEntries(entries);
	
	// save the entries
	m_mgr->GetConfigTool()->WriteObject(wxT("AbbreviationsData"), &data);
	m_activeItemName = m_textCtrlName->GetValue();
	
	// update the name in the list box
	m_listBoxAbbreviations->SetString((unsigned int) m_currSelection, m_activeItemName);
	m_dirty = false;
	
	m_textCtrlName->SetFocus();
}

void AbbreviationsSettingsDlg::DoDeleteEntry(const wxString &name)
{
	AbbreviationEntry data;
	m_mgr->GetConfigTool()->ReadObject(wxT("AbbreviationsData"), &data);
	
	// search for the old item
	std::map<wxString, wxString> entries = data.GetEntries();
	std::map<wxString, wxString>::iterator iter = entries.find(name);
	if(iter != entries.end()){
		entries.erase( iter );
	}
	data.SetEntries(entries);
	m_mgr->GetConfigTool()->WriteObject(wxT("AbbreviationsData"), &data);
}
