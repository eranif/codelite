//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : testclassbasedlg.cpp
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

#include "testclassbasedlg.h"

///////////////////////////////////////////////////////////////////////////

TestClassBaseDlg::TestClassBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Class name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlClassName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlClassName, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonClass = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonClass, 0, wxALL, 5 );
	
	m_buttonRefresh = new wxButton( this, wxID_ANY, _("&Show Functions..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonRefresh, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Fixture name (optional):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL, 5 );
	
	m_textCtrlFixtureName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textCtrlFixtureName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Output file name (optional):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlFileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlFileName->SetToolTip( _("The name of the file of which CodeLite will generate the test code.\nWhen left empty, CodeLite will use the first available source file in target project") );
	
	fgSizer2->Add( m_textCtrlFileName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Project to which these unit tests should be added:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceProjectsChoices;
	m_choiceProjects = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjectsChoices, 0 );
	m_choiceProjects->SetSelection( 0 );
	fgSizer2->Add( m_choiceProjects, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Functions to test:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer1->Add( m_staticText6, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListMethodsChoices;
	m_checkListMethods = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListMethodsChoices, 0 );
	bSizer5->Add( m_checkListMethods, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonCheckAll = new wxButton( this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonCheckAll, 0, wxALL, 5 );
	
	m_buttonUnCheckAll = new wxButton( this, wxID_ANY, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonUnCheckAll, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCreate = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCreate->SetDefault(); 
	bSizer4->Add( m_buttonCreate, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonClass->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnShowClassListDialog ), NULL, this );
	m_buttonRefresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnRefreshFunctions ), NULL, this );
	m_buttonRefresh->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TestClassBaseDlg::OnRefreshButtonUI ), NULL, this );
	m_buttonCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUnCheckAll ), NULL, this );
	m_buttonCreate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnButtonOk ), NULL, this );
}

TestClassBaseDlg::~TestClassBaseDlg()
{
	// Disconnect Events
	m_buttonClass->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnShowClassListDialog ), NULL, this );
	m_buttonRefresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnRefreshFunctions ), NULL, this );
	m_buttonRefresh->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TestClassBaseDlg::OnRefreshButtonUI ), NULL, this );
	m_buttonCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnUnCheckAll ), NULL, this );
	m_buttonCreate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TestClassBaseDlg::OnButtonOk ), NULL, this );
	
}
