///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
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

#include "envvar_table.h"
#include "manager.h"
#include "macros.h"
#include "envvar_dlg.h"
#include "globals.h"

///////////////////////////////////////////////////////////////////////////

EnvVarsTableDlg::EnvVarsTableDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_listVarsTable = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer12->Add( m_listVarsTable, 1, wxALL|wxEXPAND, 5 );
	InitVars();

	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer12->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonNew = new wxButton( this, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonNew, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer13, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();

	ConnectEvents();
}

void EnvVarsTableDlg::ConnectEvents()
{
	ConnectButton(m_buttonDelete, EnvVarsTableDlg::OnDeleteVar);
	ConnectButton(m_buttonNew, EnvVarsTableDlg::OnNewVar);
	ConnectListCtrlItemSelected(m_listVarsTable, EnvVarsTableDlg::OnItemSelected);
	ConnectListCtrlItemActivated(m_listVarsTable, EnvVarsTableDlg::OnItemActivated);
}

void EnvVarsTableDlg::InitVars()
{
	m_listVarsTable->ClearAll();
	m_listVarsTable->Freeze();
	EnvironmentVarieblesPtr env = ManagerST::Get()->GetEnvironmentVariables();
	//add two columns to the list ctrl
	m_listVarsTable->InsertColumn(0, wxT("Name"));
	m_listVarsTable->InsertColumn(1, wxT("Value"));

	EnvironmentVariebles::ConstIterator iter = env->Begin();
	for (; iter != env->End(); iter++) {
		long item = AppendListCtrlRow(m_listVarsTable);
		SetColumnText(m_listVarsTable, item, 0, iter->first);
		SetColumnText(m_listVarsTable, item, 1, iter->second);
	}
	
	m_listVarsTable->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_listVarsTable->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listVarsTable->Thaw();
}

void EnvVarsTableDlg::OnEditVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg *dlg = new EnvVarDlg(this, m_selectedVarName, m_selectedVarValue);
	if(dlg->ShowModal() == wxID_OK){
		EnvironmentVarieblesPtr env = ManagerST::Get()->GetEnvironmentVariables();
		env->SetEnv(dlg->GetName(), dlg->GetValue());
		ManagerST::Get()->SetEnvironmentVariables(env);
		InitVars();
	}
	dlg->Destroy();
}

void EnvVarsTableDlg::OnNewVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg *dlg = new EnvVarDlg(this);
	if(dlg->ShowModal() == wxID_OK){
		EnvironmentVarieblesPtr env = ManagerST::Get()->GetEnvironmentVariables();
		env->SetEnv(dlg->GetName(), dlg->GetValue());
		ManagerST::Get()->SetEnvironmentVariables(env);
		InitVars();
	}
	dlg->Destroy();
}

void EnvVarsTableDlg::OnDeleteVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString msg;
	msg << wxT("Delete environment variable '") << m_selectedVarName << wxT("'?");
	if(wxMessageBox(msg, wxT("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES){
		EnvironmentVarieblesPtr env = ManagerST::Get()->GetEnvironmentVariables();
		env->DeleteEnv(m_selectedVarName);
		ManagerST::Get()->SetEnvironmentVariables(env);
		InitVars();
	}
}

void EnvVarsTableDlg::OnItemSelected(wxListEvent &event)
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

void EnvVarsTableDlg::OnItemActivated(wxListEvent &event)
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
