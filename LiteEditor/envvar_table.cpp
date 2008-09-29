//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : envvar_table.cpp               
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
#include "evnvarlist.h"
#include "manager.h"
#include "environmentconfig.h"
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
	
	m_buttonNew = new wxButton( this, wxID_ANY, wxT("&New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonNew, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_editButton = new wxButton( this, wxID_ANY, wxT("&Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_editButton, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer13, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();
	
	//select the first item in the list
	if(m_listVarsTable->GetItemCount() > 0) {
		m_listVarsTable->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		m_listVarsTable->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
	}

	m_listVarsTable->SetFocus();
	ConnectEvents();
	// center the dialog
	Centre();
	
}

void EnvVarsTableDlg::ConnectEvents()
{
	ConnectButton(m_buttonDelete, EnvVarsTableDlg::OnDeleteVar);
	ConnectButton(m_buttonNew, EnvVarsTableDlg::OnNewVar);
	ConnectButton(m_editButton, EnvVarsTableDlg::OnEditVar)
	ConnectListCtrlItemSelected(m_listVarsTable, EnvVarsTableDlg::OnItemSelected);
	ConnectListCtrlItemActivated(m_listVarsTable, EnvVarsTableDlg::OnItemActivated);
}

void EnvVarsTableDlg::InitVars()
{
	m_listVarsTable->ClearAll();
	m_listVarsTable->Freeze();
	
	m_listVarsTable->InsertColumn(0, wxT("Name"));
	m_listVarsTable->InsertColumn(1, wxT("Value"));

	//read all environment from configuraion
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
	StringMap::const_iterator iter = vars.GetVariables().begin();
	
	for(; iter != vars.GetVariables().end(); iter++) {
		wxString name  = iter->first;
		wxString value = iter->second;
		
		//add item to the list
		long item = AppendListCtrlRow(m_listVarsTable);
		SetColumnText(m_listVarsTable, item, 0, iter->first);
		SetColumnText(m_listVarsTable, item, 1, iter->second);
	}
	
	if(vars.GetVariables().size() > 0) {
		m_listVarsTable->SetColumnWidth(0, wxLIST_AUTOSIZE);
		m_listVarsTable->SetColumnWidth(1, wxLIST_AUTOSIZE);
	}else{
		m_listVarsTable->SetColumnWidth(0, 150);
		m_listVarsTable->SetColumnWidth(1, 150);
	}
	m_listVarsTable->Thaw();
}

void EnvVarsTableDlg::OnEditVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg dlg(this);//, m_selectedVarName, m_selectedVarValue);
	dlg.SetStaticText1(wxT("Variable Name:"));
	dlg.SetStaticText1(wxT("Variable Value:"));
	dlg.SetName(m_selectedVarName);
	dlg.SetValue(m_selectedVarValue);
	
	if(dlg.ShowModal() == wxID_OK){
		EvnVarList vars;
		EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
		
		StringMap varMap = vars.GetVariables();
		varMap[dlg.GetName()] = dlg.GetValue();
		
		vars.SetVariables( varMap );
		EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
		
		InitVars();
	}
}

void EnvVarsTableDlg::OnNewVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg *dlg = new EnvVarDlg(this);
	if(dlg->ShowModal() == wxID_OK){
		EvnVarList vars;
		EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
		
		StringMap varMap = vars.GetVariables();
		varMap[dlg->GetName()] = dlg->GetValue();
		
		vars.SetVariables( varMap );
		EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
		
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
		EvnVarList vars;
		EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);
		
		StringMap varMap = vars.GetVariables();
		StringMap::iterator it = varMap.find(m_selectedVarName);
		if(it != varMap.end()){
			varMap.erase(it);
		}

		vars.SetVariables( varMap );
		EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);
		
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
