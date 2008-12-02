//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editorsettingscommentsbase.cpp              
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

#include "editorsettingscommentsbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsCommentsBase::EditorSettingsCommentsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_checkBoxContCComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C style comment section,\nautomatically add '*' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxContCComment->SetValue(true);

	mainSizer->Add( m_checkBoxContCComment, 0, wxALL|wxEXPAND, 5 );

	m_checkBoxContinueCppComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C++ style comment section,\nautomatically add '//' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );

	mainSizer->Add( m_checkBoxContinueCppComment, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCommentsBase::~EditorSettingsCommentsBase()
{
}
