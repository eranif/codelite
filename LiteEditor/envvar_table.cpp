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
#include "windowattrmanager.h"
#include "evnvarlist.h"
#include "manager.h"
#include "environmentconfig.h"
#include "macros.h"
#include "envvar_dlg.h"
#include "globals.h"

///////////////////////////////////////////////////////////////////////////

EnvVarsTableDlg::EnvVarsTableDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style )
		, m_selectedItem(wxNOT_FOUND)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_listVarsTable = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	m_listVarsTable->SetMinSize( wxSize( 300,300 ) );

	bSizer2->Add( m_listVarsTable, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_buttonNew = new wxButton( this, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonNew, 0, wxALL, 5 );

	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonDelete, 0, wxALL, 5 );

	m_editButton = new wxButton( this, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_editButton, 0, wxALL, 5 );

	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );

	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );

	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault();
	bSizer3->Add( m_buttonOk, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );

	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_listVarsTable->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( EnvVarsTableDlg::OnItemActivated ), NULL, this );
	m_listVarsTable->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( EnvVarsTableDlg::OnItemSelected ), NULL, this );
	m_buttonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlg::OnNewVar ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlg::OnDeleteVar ), NULL, this );
	m_buttonDelete->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EnvVarsTableDlg::OnSelectionValid ), NULL, this );
	m_editButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlg::OnEditVar ), NULL, this );
	m_editButton->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EnvVarsTableDlg::OnSelectionValid ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlg::OnButtonOk ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EnvVarsTableDlg::OnButtonCancel ), NULL, this );

	InitVars();
	//select the first item in the list
	if (m_listVarsTable->GetItemCount() > 0) {
		m_listVarsTable->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
		m_listVarsTable->SetItemState(0, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
	}

	m_listVarsTable->SetFocus();
	WindowAttrManager::Load(this, wxT("EnvVarsTableDlgAttr"), NULL);
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

	for (; iter != vars.GetVariables().end(); iter++) {
		wxString name  = iter->first;
		wxString value = iter->second;

		//add item to the list
		long item = AppendListCtrlRow(m_listVarsTable);
		SetColumnText(m_listVarsTable, item, 0, iter->first);
		SetColumnText(m_listVarsTable, item, 1, iter->second);
	}

	m_listVarsTable->SetColumnWidth(0, 200);
	m_listVarsTable->SetColumnWidth(1, 200);
	m_listVarsTable->Thaw();
}

void EnvVarsTableDlg::OnEditVar(wxCommandEvent &event)
{
	if (m_selectedItem != wxNOT_FOUND) {
		DoEditItem(m_selectedItem);
	}
}

void EnvVarsTableDlg::OnNewVar(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarDlg *dlg = new EnvVarDlg(this);
	if (dlg->ShowModal() == wxID_OK) {
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
	if (m_selectedItem != wxNOT_FOUND) {
		wxString msg;
		msg << wxT("Delete environment variable '") << GetColumnText(m_listVarsTable, m_selectedItem, 0) << wxT("'?");
		if (wxMessageBox(msg, wxT("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
			m_listVarsTable->DeleteItem(m_selectedItem);
			m_selectedItem = wxNOT_FOUND;
		}
	}
}

void EnvVarsTableDlg::OnItemSelected(wxListEvent &event)
{
	m_selectedItem = event.m_itemIndex;
}

void EnvVarsTableDlg::OnItemActivated(wxListEvent &event)
{
	DoEditItem(event.m_itemIndex);
}

void EnvVarsTableDlg::DoEditItem(long item)
{
	EnvVarDlg dlg(this);
	dlg.SetStaticText1(wxT("Variable Name:"));
	dlg.SetStaticText1(wxT("Variable Value:"));

	dlg.SetName(GetColumnText(m_listVarsTable, item, 0));
	dlg.SetValue(GetColumnText(m_listVarsTable, item, 1));

	if (dlg.ShowModal() == wxID_OK) {
		SetColumnText(m_listVarsTable, item, 0, dlg.GetName());
		SetColumnText(m_listVarsTable, item, 1, dlg.GetValue());

		m_listVarsTable->SetColumnWidth(0, 200);
		m_listVarsTable->SetColumnWidth(1, 200);
	}
}

void EnvVarsTableDlg::OnButtonCancel(wxCommandEvent& e)
{
	wxUnusedVar( e );
	EndModal( wxID_CANCEL);
}

void EnvVarsTableDlg::OnButtonOk(wxCommandEvent& e)
{
	wxUnusedVar( e );
	EvnVarList vars;
	EnvironmentConfig::Instance()->ReadObject(wxT("Variables"), &vars);

	StringMap varMap;
	for (int i=0; i<m_listVarsTable->GetItemCount(); i++)	{
		wxString name = GetColumnText(m_listVarsTable, i, 0);
		wxString value = GetColumnText(m_listVarsTable, i, 1);
		varMap[name] = value;
	}

	vars.SetVariables( varMap );
	EnvironmentConfig::Instance()->WriteObject(wxT("Variables"), &vars);

	EndModal(wxID_OK);
}

void EnvVarsTableDlg::OnSelectionValid(wxUpdateUIEvent& event)
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}

EnvVarsTableDlg::~EnvVarsTableDlg()
{
	WindowAttrManager::Save(this, wxT("EnvVarsTableDlgAttr"), NULL);
}
