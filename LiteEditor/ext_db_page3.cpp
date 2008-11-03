//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : ext_db_page3.cpp              
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
// C++ code generated with wxFormBuilder (version Jul 28 2007)
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
#include "macros.h"
#include "ext_db_page3.h"
#include "manager.h"
#include "wx/dir.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(ExtDbPage3, wxWizardPageSimple)
EVT_WIZARD_PAGE_CHANGING(wxID_ANY, ExtDbPage3::OnValidate)
END_EVENT_TABLE()

ExtDbPage3::ExtDbPage3(wxWizard* parent)
: wxWizardPageSimple(parent)
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Please enter a name for the symbol database:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	//Populate the combo box with an existing tags database files
	wxArrayString files;
	wxArrayString shortNames;
	wxString fileSpec( wxT( "*.tags" ) );
	wxDir::GetAllFiles( ManagerST::Get()->GetStarupDirectory(), &files, fileSpec, wxDIR_FILES );
	
	for(size_t i=0; i<files.GetCount(); i++){
		wxFileName fn(files.Item(i));
		shortNames.Add(fn.GetFullName());
	}
	
	wxString selection = TagsManagerST::Get()->GetExtDatabase()->GetDatabaseFileName().GetFullName();
	m_comboDbName = new wxComboBox(m_panel1, wxID_ANY, selection, wxDefaultPosition, wxDefaultSize, shortNames);
	
	bSizer2->Add( m_comboDbName, 0, wxALL|wxEXPAND, 5 );
	
	m_checkSetAsActive = new wxCheckBox(m_panel1, wxID_ANY, wxT("Attach the database when completed"));
	m_checkSetAsActive->SetValue(true);
	bSizer2->Add( m_checkSetAsActive, 0, wxALL, 5 );

	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 0 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxEmptyString ), wxVERTICAL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Parsed symbols are stored into this database.\nIf a database with this name already exist, the new symbols are appended"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer1->Add( m_staticText3, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}

void ExtDbPage3::OnValidate(wxWizardEvent &event)
{
	//validate that there is a valid name to the database
	wxString dbname(m_comboDbName->GetValue());
	TrimString(dbname);
	if(dbname.IsEmpty() && event.GetDirection()){
		wxMessageBox(_("Database name can not be empty"));
		event.Veto();
		return;
	}
	
	if(!dbname.EndsWith(wxT(".tags"))){
		dbname << wxT(".tags");
	}
	m_comboDbName->SetValue(dbname);
}

