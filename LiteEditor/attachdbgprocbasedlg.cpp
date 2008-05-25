//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : attachdbgprocbasedlg.cpp              
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
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "attachdbgprocbasedlg.h"

///////////////////////////////////////////////////////////////////////////
 
AttachDbgProcBaseDlg::AttachDbgProcBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Debugger to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDebuggerChoices;
	m_choiceDebugger = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggerChoices, 0 );
	fgSizer1->Add( m_choiceDebugger, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Processes:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer1->Add( m_staticText4, 0, wxALL, 5 );
	
	m_listCtrlProcesses = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer1->Add( m_listCtrlProcesses, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&Attach"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button2, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listCtrlProcesses->Connect( wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler( AttachDbgProcBaseDlg::OnSortColumn ), NULL, this );
	m_listCtrlProcesses->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( AttachDbgProcBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrlProcesses->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( AttachDbgProcBaseDlg::OnItemDeselected ), NULL, this );
	m_listCtrlProcesses->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( AttachDbgProcBaseDlg::OnItemSelected ), NULL, this );
	m_buttonOk->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( AttachDbgProcBaseDlg::OnBtnAttachUI ), NULL, this );
}
