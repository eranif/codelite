//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : externaltoolsbasedlg.cpp
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

#include "externaltoolsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

ExternalToolBaseDlg::ExternalToolBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	bSizer6->SetMinSize( wxSize( 700,250 ) ); 
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlTools = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer8->Add( m_listCtrlTools, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewTool = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_buttonNewTool, 0, wxALL, 5 );
	
	m_buttonEdit = new wxButton( this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_buttonEdit, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, _("Delete..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_buttonDelete, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer9, 0, 0, 5 );
	
	bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer6->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer7->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer7, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listCtrlTools->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ExternalToolBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrlTools->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ExternalToolBaseDlg::OnItemDeSelected ), NULL, this );
	m_listCtrlTools->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ExternalToolBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewTool->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonNew ), NULL, this );
	m_buttonNewTool->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonNewUI ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonEdit ), NULL, this );
	m_buttonEdit->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonEditUI ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonDelete ), NULL, this );
	m_buttonDelete->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonDeleteUI ), NULL, this );
}

ExternalToolBaseDlg::~ExternalToolBaseDlg()
{
	// Disconnect Events
	m_listCtrlTools->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ExternalToolBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrlTools->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( ExternalToolBaseDlg::OnItemDeSelected ), NULL, this );
	m_listCtrlTools->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ExternalToolBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewTool->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonNew ), NULL, this );
	m_buttonNewTool->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonNewUI ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonEdit ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonEditUI ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ExternalToolBaseDlg::OnButtonDelete ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ExternalToolBaseDlg::OnButtonDeleteUI ), NULL, this );
	
}
