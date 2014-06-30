//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : batchbuildbasedlg.cpp
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
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "batchbuildbasedlg.h"

///////////////////////////////////////////////////////////////////////////

BatchBuildBaseDlg::BatchBuildBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText = new wxStaticText( this, wxID_ANY, _("Select the configurations to build/clean:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText->Wrap( -1 );
	bSizer1->Add( m_staticText, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListConfigurationsChoices;
	m_checkListConfigurations = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListConfigurationsChoices, 0 );
	bSizer2->Add( m_checkListConfigurations, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonBuild = new wxButton( this, wxID_ANY, _("&Build"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonBuild->SetDefault(); 
	bSizer4->Add( m_buttonBuild, 0, wxALL, 5 );
	
	m_buttonClean = new wxButton( this, wxID_ANY, _("&Clean"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonClean, 0, wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL, 5 );
	
	m_button4 = new wxButton( this, wxID_ANY, _("&Uncheck All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button4, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonDown = new wxButton( this, wxID_ANY, _("Move Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonDown, 0, wxALL, 5 );
	
	m_buttonUp = new wxButton( this, wxID_ANY, _("Move Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonUp, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer5, 1, wxEXPAND, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonClose = new wxButton( this, wxID_CANCEL, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonClose, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer6, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkListConfigurations->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BatchBuildBaseDlg::OnItemSelected ), NULL, this );
	m_checkListConfigurations->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( BatchBuildBaseDlg::OnItemToggled ), NULL, this );
	m_buttonBuild->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnBuild ), NULL, this );
	m_buttonBuild->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnBuildUI ), NULL, this );
	m_buttonClean->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnClean ), NULL, this );
	m_buttonClean->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnCleanUI ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnCheckAll ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnUnCheckAll ), NULL, this );
	m_buttonDown->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnMoveUp ), NULL, this );
	m_buttonDown->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnMoveUpUI ), NULL, this );
	m_buttonUp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnMoveDown ), NULL, this );
	m_buttonUp->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnMoveDownUI ), NULL, this );
	m_buttonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnClose ), NULL, this );
}

BatchBuildBaseDlg::~BatchBuildBaseDlg()
{
	// Disconnect Events
	m_checkListConfigurations->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( BatchBuildBaseDlg::OnItemSelected ), NULL, this );
	m_checkListConfigurations->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( BatchBuildBaseDlg::OnItemToggled ), NULL, this );
	m_buttonBuild->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnBuild ), NULL, this );
	m_buttonBuild->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnBuildUI ), NULL, this );
	m_buttonClean->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnClean ), NULL, this );
	m_buttonClean->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnCleanUI ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnCheckAll ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnUnCheckAll ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnMoveUp ), NULL, this );
	m_buttonDown->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnMoveUpUI ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnMoveDown ), NULL, this );
	m_buttonUp->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BatchBuildBaseDlg::OnMoveDownUI ), NULL, this );
	m_buttonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BatchBuildBaseDlg::OnClose ), NULL, this );
	
}
