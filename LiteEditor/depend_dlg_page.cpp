//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : depend_dlg_page.cpp              
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

#include "depend_dlg_page.h"
#include "manager.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////

DependenciesPage::DependenciesPage( wxWindow* parent, const wxString &projectName, int id, wxPoint pos, wxSize size, int style ) 
: wxPanel( parent, id, pos, size, style )
, m_projectName(projectName)
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Projects:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_staticText1, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_checkListProjectListChoices;
	m_checkListProjectList = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListProjectListChoices, 0 );
	bSizer3->Add( m_checkListProjectList, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Build Order:"), wxDefaultPosition, wxDefaultSize, 0 );

	wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
	bSizer3->Add( m_staticText2, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	bSizer3->Add(sz, 1, wxALL|wxEXPAND);
	wxBoxSizer *btnSizer = new wxBoxSizer(wxVERTICAL);

	m_listBoxBuildOrder = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sz->Add(m_listBoxBuildOrder, 1, wxALL|wxEXPAND, 5);
	sz->Add(btnSizer);
	
	wxButton *upBtn = new wxButton(this, wxID_ANY, wxT("Up"));
	btnSizer->Add(upBtn, 0, wxALL | wxALIGN_TOP, 5);
	ConnectButton(upBtn, DependenciesPage::OnMoveUp);

	wxButton *downBtn = new wxButton(this, wxID_ANY, wxT("Down"));
	btnSizer->Add(downBtn, 0, wxALL, 5);
	ConnectButton(downBtn, DependenciesPage::OnMoveDown);

	mainSizer->Add( bSizer3, 1, wxEXPAND, 5 );
	this->SetSizer( mainSizer );
	this->Layout();
	Init();
}

void DependenciesPage::Init()
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
	if(proj){
		//initialize the build order listbox
		wxArrayString depArr = proj->GetDependencies();
		size_t i=0;
		for(i=0; i<depArr.GetCount(); i++){
			wxString item = depArr.Item(i);
			m_listBoxBuildOrder->Append(item);
		}
		//initialize the project dependencies check list
		wxArrayString projArr;
		ManagerST::Get()->GetProjectList(projArr);

		for(i=0; i<projArr.GetCount(); i++){

			if(projArr.Item(i) != m_projectName){
				int idx = m_checkListProjectList->Append(projArr.Item(i));
				m_checkListProjectList->Check(idx, depArr.Index(projArr.Item(i)) != wxNOT_FOUND);
			}
		}

	}else{
		wxMessageBox(errMsg, wxT("CodeLite"));
		return;
	}

	ConnectCheckList(m_checkListProjectList, DependenciesPage::OnCheckListItemToggled);
}

void DependenciesPage::OnCheckListItemToggled(wxCommandEvent &event)
{
	int item = event.GetSelection();
	wxString name = m_checkListProjectList->GetString((unsigned int)item);
	if(!m_checkListProjectList->IsChecked((unsigned int)item)){
		unsigned int buildOrderId = m_listBoxBuildOrder->FindString(name);
		if(buildOrderId != (unsigned int)wxNOT_FOUND){
			m_listBoxBuildOrder->Delete(buildOrderId);
		}
	} else {
		m_listBoxBuildOrder->Append(name);
	}
}

void DependenciesPage::Save()
{
	//create project dependencie list
	ProjectPtr proj = ManagerST::Get()->GetProject(m_projectName);

	wxArrayString depsArr;
	for(size_t i=0; i<m_listBoxBuildOrder->GetCount(); i++){
		depsArr.Add(m_listBoxBuildOrder->GetString((unsigned int)i));
	}
	proj->SetDependencies(depsArr);
}

void DependenciesPage::OnMoveUp(wxCommandEvent &event)
{
	wxUnusedVar(event);
	OnUpCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnMoveDown(wxCommandEvent &event)
{
	wxUnusedVar(event);
	OnDownCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnUpCommand(wxListBox *list)
{
	wxString selectedString  = list->GetStringSelection();
	
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}
	
	sel --;
	if(sel < 0){
		return;
	}

	// sel contains the new position we want to place the selection string
	list->Delete(sel + 1);
	list->Insert(selectedString, sel);
	list->Select(sel);
}

void DependenciesPage::OnDownCommand(wxListBox *list)
{
	int sel = list->GetSelection();
	if(sel == wxNOT_FOUND){
		return;
	}

	sel ++;
	if(sel >= (int)list->GetCount()){
		return;
	}

	// sel contains the new position we want to place the selection string
	wxString oldStr = list->GetString(sel);

	list->Delete(sel);
	list->Insert(oldStr, sel - 1);
	list->Select(sel);
}
