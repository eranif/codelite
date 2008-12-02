//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingsdialogsbase.cpp              
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

#include "editorsettingsdialogsbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsDialogsBase::EditorSettingsDialogsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Below is a list of the 'annoying' dialogs answers, you can modify \nthe answer of a dialog by checking / unchecking the saved answer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	mainSizer->Add( m_staticText1, 0, wxALL, 5 );

	wxArrayString m_checkListAnswersChoices;
	m_checkListAnswers = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListAnswersChoices, 0 );
	mainSizer->Add( m_checkListAnswers, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsDialogsBase::~EditorSettingsDialogsBase()
{
}
