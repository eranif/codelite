//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debuggersettingsbasedlg.cpp              
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

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "debuggersettingsbasedlg.h"

///////////////////////////////////////////////////////////////////////////

DebuggerSettingsBaseDlg::DebuggerSettingsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel2 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrl1 = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer5->Add( m_listCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewType = new wxButton( m_panel2, wxID_ANY, wxT("&New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonNewType, 0, wxALL, 5 );
	
	m_buttonEdit = new wxButton( m_panel2, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonEdit, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( m_panel2, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_buttonDelete, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer6, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_notebook2->AddPage( m_panel2, wxT("Shortcuts"), true );
	m_panel1 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_book = new wxNotebook( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer3->Add( m_book, 1, wxEXPAND | wxALL, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_notebook2->AddPage( m_panel1, wxT("Loaded Debuggers"), false );
	
	bSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemDeselected ), NULL, this );
	m_listCtrl1->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewType->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewShortcut ), NULL, this );
	m_buttonEdit->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnEditShortcut ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteShortcut ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
}

DebuggerSettingsBaseDlg::~DebuggerSettingsBaseDlg()
{
	// Disconnect Events
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemActivated ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemDeselected ), NULL, this );
	m_listCtrl1->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( DebuggerSettingsBaseDlg::OnItemSelected ), NULL, this );
	m_buttonNewType->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnNewShortcut ), NULL, this );
	m_buttonEdit->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnEditShortcut ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnDeleteShortcut ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DebuggerSettingsBaseDlg::OnButtonCancel ), NULL, this );
}
