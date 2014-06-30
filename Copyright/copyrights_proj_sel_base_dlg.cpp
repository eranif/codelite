//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : copyrights_proj_sel_base_dlg.cpp
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

#include "copyrights_proj_sel_base_dlg.h"

///////////////////////////////////////////////////////////////////////////

CopyrightsProjectSelBaseDlg::CopyrightsProjectSelBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Select projects to insert copyrights block:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkListProjectsChoices;
	m_checkListProjects = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListProjectsChoices, 0 );
	bSizer6->Add( m_checkListProjects, 1, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( bSizer6, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonCheckAll = new wxButton( this, wxID_ANY, _("Check &All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonCheckAll, 0, wxALL, 5 );
	
	m_buttonUnCheckAll = new wxButton( this, wxID_ANY, _("&Un-Check All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonUnCheckAll, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer7, 0, 0, 5 );
	
	bSizer3->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonInsert = new wxButton( this, wxID_OK, _("&Insert"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonInsert->SetDefault(); 
	bSizer4->Add( m_buttonInsert, 0, wxALL, 5 );
	
	m_button5 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	// Connect Events
	m_buttonCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyrightsProjectSelBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyrightsProjectSelBaseDlg::OnUnCheckAll ), NULL, this );
}

CopyrightsProjectSelBaseDlg::~CopyrightsProjectSelBaseDlg()
{
	// Disconnect Events
	m_buttonCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyrightsProjectSelBaseDlg::OnCheckAll ), NULL, this );
	m_buttonUnCheckAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyrightsProjectSelBaseDlg::OnUnCheckAll ), NULL, this );
	
}
