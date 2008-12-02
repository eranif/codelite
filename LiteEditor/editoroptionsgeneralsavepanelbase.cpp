//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : editoroptionsgeneralsavepanelbase.cpp              
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

#include "editoroptionsgeneralsavepanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralSavePanelBase::EditorOptionsGeneralSavePanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_checkBoxTrimLine = new wxCheckBox( this, wxID_ANY, _("When saving files, trim empty lines"), wxDefaultPosition, wxDefaultSize, 0 );

	mainSizer->Add( m_checkBoxTrimLine, 0, wxALL, 5 );

	m_checkBoxAppendLF = new wxCheckBox( this, wxID_ANY, _("If missing, append EOL at end of file"), wxDefaultPosition, wxDefaultSize, 0 );

	mainSizer->Add( m_checkBoxAppendLF, 0, wxALL, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorOptionsGeneralSavePanelBase::~EditorOptionsGeneralSavePanelBase()
{
}
