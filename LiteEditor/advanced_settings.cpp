//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : advanced_settings.cpp              
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
#include "macros.h"
#include "buildsettingstab.h"
#include "advanced_settings.h"
#include "manager.h"
#include "compiler_page.h"
#include "editor_config.h"
#include <wx/xrc/xmlres.h>
#include "build_settings_config.h"
#include "build_page.h"

BEGIN_EVENT_TABLE(AdvancedDlg, wxDialog)
	EVT_MENU(XRCID("delete_compiler"), AdvancedDlg::OnDeleteCompiler)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////

AdvancedDlg::AdvancedDlg( wxWindow* parent, size_t selected_page, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style | wxRESIZE_BORDER )
		, m_rightclickMenu(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_compilersPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( m_compilersPage, wxID_ANY, wxT("Create New Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_buttonNewCompiler = new wxButton( m_compilersPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonNewCompiler, 0, wxALL|wxALIGN_RIGHT, 5 );

	bSizer5->Add( bSizer4, 0, wxALIGN_RIGHT|wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( m_compilersPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer5->Add( m_staticline2, 0, wxEXPAND | wxRIGHT | wxLEFT, 5 );

	m_compilersNotebook = new wxNotebook(m_compilersPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
//	m_compilersNotebook->AssignImageList(new wxImageList(16, 16));
//	long bkStyle = m_compilersNotebook->GetToolBar()->GetWindowStyleFlag();
//	m_compilersNotebook->GetToolBar()->SetWindowStyleFlag(wxTB_FLAT | wxTB_NOICONS | wxTB_TEXT | wxTB_HORIZONTAL);

	bSizer5->Add( m_compilersNotebook, 1, wxEXPAND | wxALL, 5 );

	m_compilersPage->SetSizer( bSizer5 );
	m_compilersPage->Layout();
	bSizer5->Fit( m_compilersPage );
	m_notebook->AddPage( m_compilersPage, wxT("Compilers"), true );

	mainSizer->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );

	m_staticline10 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline10, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonOK, 0, wxALL, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonCancel, 0, wxALL, 5 );

	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );

	m_buildSettings = new BuildTabSetting(m_notebook);
	m_notebook->AddPage(m_buildSettings, wxT("Build Output Appearance"), false);

	m_buildPage = new BuildPage(m_notebook);
	m_notebook->AddPage(m_buildPage, wxT("Build Systems"), false);

	this->SetSizer( mainSizer );
	this->Layout();

	m_compilersNotebook->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(AdvancedDlg::OnMouseRightUp), NULL, this);
	m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));

	LoadCompilers();
	ConnectButton(m_buttonNewCompiler, AdvancedDlg::OnButtonNewClicked);
	ConnectButton(m_buttonOK, AdvancedDlg::OnButtonOKClicked);

	m_notebook->SetSelection( selected_page );
}

void AdvancedDlg::LoadCompilers()
{
	m_compilersNotebook->Freeze();
	m_compilersNotebook->DeleteAllPages();

	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	bool first (true);
	while (cmp) {
		m_compilersNotebook->AddPage(new CompilerPage(m_compilersNotebook, cmp->GetName()), cmp->GetName(), first);
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
		first = false;
	}
	
	m_compilersNotebook->Thaw();
}

AdvancedDlg::~AdvancedDlg()
{
	delete m_rightclickMenu;
}

void AdvancedDlg::OnButtonNewClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Enter New Compiler Name:"), wxT("New Compiler"));
	if (dlg->ShowModal() == wxID_OK) {
		wxString name = dlg->GetValue();
		TrimString(name);
		if (name.IsEmpty() == false) {
			ManagerST::Get()->CreateDefaultNewCompiler(name);
			LoadCompilers();
		}
	}
	dlg->Destroy();
}

void AdvancedDlg::OnButtonOKClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//save all compiler pages
	int count = m_compilersNotebook->GetPageCount();
	for (int i=0; i<count; i++) {
		CompilerPage *page = dynamic_cast<CompilerPage*>( m_compilersNotebook->GetPage((size_t)i));
		if (page) {
			page->Save();
		}
	}
	//save the build page
	m_buildPage->Save();
	m_buildSettings->Save();

	//mark all the projects as dirty
	wxArrayString projects;
	WorkspaceST::Get()->GetProjectList( projects );
	for ( size_t i=0; i< projects.size(); i++ ) {
		ProjectPtr proj = ManagerST::Get()->GetProject( projects.Item(i) );
		if ( proj ) {
			proj->SetModified( true );
		}
	}
	EndModal(wxID_OK);
}

void AdvancedDlg::OnDeleteCompiler(wxCommandEvent & event)
{
	wxUnusedVar(event);
	int sel = m_compilersNotebook->GetSelection();

	if (sel != wxNOT_FOUND) {
		wxString name = m_compilersNotebook->GetPageText((size_t)sel);
		if (ManagerST::Get()->DeleteCompiler(name)) {
			m_compilersNotebook->DeletePage((size_t)sel);
		}
	}
}

void AdvancedDlg::OnMouseRightUp(wxMouseEvent &e)
{
	wxPoint pt = e.GetPosition();
	long flags = wxBK_HITTEST_ONLABEL;
	int tab = m_compilersNotebook->HitTest(pt, &flags);

	if (tab != wxNOT_FOUND) {
		switch (flags) {
		case wxBK_HITTEST_ONLABEL:
			PopupMenu(m_rightclickMenu);
			break;
		default:
			break;
		}
	}
}
