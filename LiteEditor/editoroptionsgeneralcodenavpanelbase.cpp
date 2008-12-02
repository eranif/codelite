//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editoroptionsgeneralcodenavpanelbase.cpp              
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
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editoroptionsgeneralcodenavpanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralCodeNavPanelBase::EditorOptionsGeneralCodeNavPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_radioBoxNavigationMethodLeft = new wxRadioButton( this, wxID_ANY, _("Ctrl+Mouse left button jumps to declaration\nCtrl+Alt+Mouse left button jumps to implementation"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_radioBoxNavigationMethodLeft->SetValue( true );
	mainSizer->Add( m_radioBoxNavigationMethodLeft, 0, wxALL, 5 );

	m_radioBoxNavigationMethodMiddle = new wxRadioButton( this, wxID_ANY, _("Mouse middle button jumps to declaration\nCtrl+Mouse middle button jumps to implementation"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_radioBoxNavigationMethodMiddle, 0, wxALL, 5 );

	bSizer2->Add( mainSizer, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer2 );
	this->Layout();
	bSizer2->Fit( this );
}

EditorOptionsGeneralCodeNavPanelBase::~EditorOptionsGeneralCodeNavPanelBase()
{
}
