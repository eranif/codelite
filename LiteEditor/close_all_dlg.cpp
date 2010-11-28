//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : close_all_dlg.cpp              
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

#include "close_all_dlg.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////

CloseAllDialog::CloseAllDialog( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticMsg = new wxStaticText( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_staticMsg, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonSave = new wxButton( m_mainPanel, wxID_ANY, _("Ask me for each file"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonSave, 0, wxALL, 5 );
	
	m_buttonSaveAllFiles = new wxButton( m_mainPanel, wxID_ANY, _("Save all files"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonSaveAllFiles, 0, wxALL, 5 );
	
	m_buttonDiscardChangesForAllFiles = new wxButton( m_mainPanel, wxID_ANY, _("Discard changes for all files"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonDiscardChangesForAllFiles, 0, wxALL, 5 );
	
	panelSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_mainPanel->SetSizer( panelSizer );
	m_mainPanel->Layout();
	panelSizer->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_staticMsg->SetLabel(_("Some of the files are modified, what action should CodeLite take?"));
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit(this);

	ConnectButton(m_buttonDiscardChangesForAllFiles, CloseAllDialog::OnDiscardAllClicked);
	ConnectButton(m_buttonSaveAllFiles, CloseAllDialog::OnSaveAll);
	ConnectButton(m_buttonSave, CloseAllDialog::OnAskForEachFile);
}

void CloseAllDialog::OnDiscardAllClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(CLOSEALL_DISCARDALL);
}

void CloseAllDialog::OnSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(CLOSEALL_SAVEALL);
}

void CloseAllDialog::OnAskForEachFile(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(CLOSEALL_ASKFOREACHFILE);
}
