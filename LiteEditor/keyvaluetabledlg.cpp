///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "keyvaluetabledlg.h"
#include "macros.h"
#include "ctags_manager.h"
#include "envvar_dlg.h"

///////////////////////////////////////////////////////////////////////////

KeyValueTableDlg::KeyValueTableDlg( wxWindow* parent, TagsDatabase *db, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
, m_db(db)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_listVarsTable = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer12->Add( m_listVarsTable, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer12->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer13, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();

	ConnectEvents();
	InitVars();
}

void KeyValueTableDlg::ConnectEvents()
{
	ConnectListCtrlItemSelected(m_listVarsTable, KeyValueTableDlg::OnItemSelected);
	ConnectListCtrlItemActivated(m_listVarsTable, KeyValueTableDlg::OnItemActivated);
}

void KeyValueTableDlg::InitVars()
{
	m_listVarsTable->ClearAll();
	m_listVarsTable->Freeze();
	
	std::vector<VariableEntryPtr> vars;
	m_db->GetVariables(vars);
	
	//add two columns to the list ctrl
	m_listVarsTable->InsertColumn(0, wxT("Name"));
	m_listVarsTable->InsertColumn(1, wxT("Value"));
	bool sel = true;

	for(size_t i=0; i<vars.size(); i++){
		long item;
		wxListItem info;
		// Set the item display name
		info.SetText(vars.at(i)->GetName());
		info.SetColumn(0);
		if(sel == true){
			sel = false;
			info.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
			m_selectedVarName = vars.at(i)->GetName();
			m_selectedVarValue = vars.at(i)->GetValue();
			item = m_listVarsTable->InsertItem(info);
		}else{
			info.SetState(0);
			item = m_listVarsTable->InsertItem(info);
		}

		info.SetColumn(1);
		info.SetId(item);
		info.SetText(vars.at(i)->GetValue());
		info.SetState(0);
		m_listVarsTable->SetItem(info); 
	}

	m_listVarsTable->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_listVarsTable->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listVarsTable->Thaw();
}

void KeyValueTableDlg::OnEditVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg *dlg = new EnvVarDlg(this, m_selectedVarName, m_selectedVarValue);
	if(dlg->ShowModal() == wxID_OK){
		DbRecordPtr record(new VariableEntry(dlg->GetName(), dlg->GetValue()));
		m_db->Begin();
		m_db->Update(record);
		m_db->Commit();
		InitVars();
	}
	dlg->Destroy();
}

void KeyValueTableDlg::OnItemSelected(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listVarsTable->GetItem(info) ){
		m_selectedVarName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listVarsTable->GetItem(info)){
		m_selectedVarValue = info.m_text;
	}
}

void KeyValueTableDlg::OnItemActivated(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listVarsTable->GetItem(info) ){
		m_selectedVarName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listVarsTable->GetItem(info)){
		m_selectedVarValue = info.m_text;
	}

	wxCommandEvent dummy;
	OnEditVar(dummy);
}
