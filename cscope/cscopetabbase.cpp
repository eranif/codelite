//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cscopetabbase.cpp              
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
// C++ code generated with wxFormBuilder (version Feb 20 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cscopetabbase.h"

///////////////////////////////////////////////////////////////////////////

CscopeTabBase::CscopeTabBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_statusMessage = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_statusMessage->Wrap( -1 );
	mainSizer->Add( m_statusMessage, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_treeCtrlResults = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	mainSizer->Add( m_treeCtrlResults, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetValue( 0 ); 
	bSizer3->Add( m_gauge, 1, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 0 );
	
	m_buttonClear = new wxButton( this, wxID_ANY, wxT("&Clear Results"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonClear, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( bSizer3, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_treeCtrlResults->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CscopeTabBase::OnClearResults ), NULL, this );
}

CscopeTabBase::~CscopeTabBase()
{
	// Disconnect Events
	m_treeCtrlResults->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CscopeTabBase::OnClearResults ), NULL, this );
}
