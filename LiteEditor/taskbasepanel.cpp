//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : taskbasepanel.cpp              
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
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "taskbasepanel.h"

///////////////////////////////////////////////////////////////////////////

TaskBasePanel::TaskBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 7, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 3 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Filter:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceFilterChoices[] = { wxT("All"), wxT("TODO"), wxT("FIXME"), wxT("BUG"), wxT("ATTN") };
	int m_choiceFilterNChoices = sizeof( m_choiceFilterChoices ) / sizeof( wxString );
	m_choiceFilter = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceFilterNChoices, m_choiceFilterChoices, 0 );
	m_choiceFilter->SetSelection( 0 );
	fgSizer1->Add( m_choiceFilter, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Scope:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceScopeChoices[] = { wxT("Entire Workspace"), wxT("Active Project") };
	int m_choiceScopeNChoices = sizeof( m_choiceScopeChoices ) / sizeof( wxString );
	m_choiceScope = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceScopeNChoices, m_choiceScopeChoices, 0 );
	m_choiceScope->SetSelection( 0 );
	fgSizer1->Add( m_choiceScope, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonSearch = new wxButton( this, wxID_ANY, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSearch->SetDefault(); 
	fgSizer1->Add( m_buttonSearch, 0, wxALL, 5 );
	
	m_buttonStop = new wxButton( this, wxID_ANY, wxT("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonStop, 0, wxALL, 5 );
	
	m_buttonClear = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonClear, 0, wxALL, 5 );
	
	bSizer5->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_listCtrlTasks = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer5->Add( m_listCtrlTasks, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_choiceFilter->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TaskBasePanel::OnFilter ), NULL, this );
	m_buttonSearch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnSearch ), NULL, this );
	m_buttonStop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnStop ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnClear ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TaskBasePanel::OnClearUI ), NULL, this );
	m_listCtrlTasks->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TaskBasePanel::OnItemActivated ), NULL, this );
}

TaskBasePanel::~TaskBasePanel()
{
	// Disconnect Events
	m_choiceFilter->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( TaskBasePanel::OnFilter ), NULL, this );
	m_buttonSearch->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnSearch ), NULL, this );
	m_buttonStop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnStop ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TaskBasePanel::OnClear ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TaskBasePanel::OnClearUI ), NULL, this );
	m_listCtrlTasks->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TaskBasePanel::OnItemActivated ), NULL, this );
}
