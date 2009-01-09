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

#include "windowattrmanager.h"
#include "macros.h"
#include "buildsettingstab.h"
#include "advanced_settings.h"
#include "manager.h"
#include "editor_config.h"
#include <wx/xrc/xmlres.h>
#include "build_settings_config.h"
#include "build_page.h"
#include "compilerswitchespage.h"
#include "compilertoolspage.h"
#include "compilerpatternspage.h"
#include "compileradvancepage.h"
#include "compilerfiletypespage.h"

BEGIN_EVENT_TABLE(AdvancedDlg, wxDialog)
	EVT_MENU(XRCID("delete_compiler"), AdvancedDlg::OnDeleteCompiler)
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////

AdvancedDlg::AdvancedDlg( wxWindow* parent, size_t selected_page, int id, wxString title, wxPoint pos, wxSize size, int style )
		: wxDialog( parent, id, title, pos, size, style | wxRESIZE_BORDER )
		, m_rightclickMenu(NULL)
{
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

	m_compilersNotebook = new wxTreebook(m_compilersPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );
	bSizer5->Add( m_compilersNotebook, 1, wxALL|wxEXPAND, 5 );

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

	m_compilersNotebook->GetTreeCtrl()->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(AdvancedDlg::OnContextMenu), NULL, this);
	m_rightclickMenu = wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu"));

	LoadCompilers();

	if(m_compilersNotebook->GetPageCount()){
		m_compilersNotebook->ExpandNode(0);
		m_compilersNotebook->SetSelection(1);
	}

	ConnectButton(m_buttonNewCompiler, AdvancedDlg::OnButtonNewClicked);
	ConnectButton(m_buttonOK, AdvancedDlg::OnButtonOKClicked);

	m_notebook->SetSelection( selected_page );

	// center the dialog
	Centre();
	this->Layout();
	GetSizer()->Fit(this);

	m_compilersNotebook->SetFocus();
	WindowAttrManager::Load(this, wxT("BuildSettingsDlg"), NULL);
}

void AdvancedDlg::LoadCompilers()
{
	m_compilerPagesMap.clear();
	m_compilersNotebook->Freeze();
	m_compilersNotebook->DeleteAllPages();

	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	bool first (true);
	while (cmp) {
		AddCompiler(cmp, first);
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
		first = false;
	}

	m_compilersNotebook->Thaw();
}

AdvancedDlg::~AdvancedDlg()
{
	delete m_rightclickMenu;
	WindowAttrManager::Save(this, wxT("BuildSettingsDlg"), NULL);
}

void AdvancedDlg::OnButtonNewClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Enter New Compiler Name:"), wxT("New Compiler"));
	if (dlg->ShowModal() == wxID_OK) {
		wxString name = dlg->GetValue();
		TrimString(name);
		if (name.IsEmpty() == false) {
			CreateDefaultNewCompiler(name);
			LoadCompilers();

			if(m_compilersNotebook->GetPageCount() > ((m_compilerPagesMap.size() *6)-1) ) {
				int start_pos = (m_compilerPagesMap.size()-1)*6;
				m_compilersNotebook->ExpandNode(start_pos);
				m_compilersNotebook->SetSelection(start_pos + 1);
			}
			SetSizerAndFit(GetSizer());
		}
	}
	dlg->Destroy();
}

void AdvancedDlg::OnButtonOKClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	//save all compiler pages
	SaveCompilers();

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
		if (DeleteCompiler(name)) {
			m_compilersNotebook->DeletePage((size_t)sel);

			std::map<wxString, std::vector<ICompilerSubPage*> >::iterator iter = m_compilerPagesMap.find(name);
			if(iter != m_compilerPagesMap.end()){
				m_compilerPagesMap.erase(iter);
			}

			if(m_compilersNotebook->GetPageCount()){
				m_compilersNotebook->ExpandNode(0);
				m_compilersNotebook->SetSelection(1);
			}
			SetSizerAndFit(GetSizer());
		}
	}
}

void AdvancedDlg::SaveCompilers()
{
	std::map<wxString, std::vector<ICompilerSubPage*> >::iterator iter = m_compilerPagesMap.begin();
	for(; iter != m_compilerPagesMap.end(); iter ++){
		std::vector<ICompilerSubPage*> items = iter->second;
		wxString cmpname = iter->first;
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(cmpname);
		if(cmp){
			for(size_t i=0; i<items.size(); i++){
				ICompilerSubPage* p = items.at(i);
				p->Save(cmp);
			}
			BuildSettingsConfigST::Get()->SetCompiler(cmp);//save changes
		}
	}
}

bool AdvancedDlg::CreateDefaultNewCompiler ( const wxString &name )
{
	if ( BuildSettingsConfigST::Get()->IsCompilerExist ( name ) ) {
		wxMessageBox ( _ ( "A compiler with this name already exist" ), wxT ( "Error" ), wxOK | wxICON_HAND );
		return false;
	}

	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler ( name );
	cmp->SetName ( name );
	BuildSettingsConfigST::Get()->SetCompiler ( cmp );
	return true;
}

void AdvancedDlg::AddCompiler(CompilerPtr cmp, bool selected)
{
	std::vector<ICompilerSubPage*> pages;

	// add the root node
	m_compilersNotebook->AddPage(0, cmp->GetName(), selected);

	CompilerPatternsPage *p2 = new CompilerPatternsPage(m_compilersNotebook, cmp->GetName());
	pages.push_back(p2);
	m_compilersNotebook->AddSubPage(p2, _("Patterns"), false);

	CompilerToolsPage *p3 = new CompilerToolsPage(m_compilersNotebook, cmp->GetName());
	pages.push_back(p3);
	m_compilersNotebook->AddSubPage(p3, _("Tools"), false);

	CompilerSwitchesPage *p4 = new CompilerSwitchesPage(m_compilersNotebook, cmp->GetName());
	pages.push_back(p4);
	m_compilersNotebook->AddSubPage(p4, _("Switches"), false);

	CompilerFileTypePage *p5 = new CompilerFileTypePage(m_compilersNotebook, cmp->GetName());
	pages.push_back(p5);
	m_compilersNotebook->AddSubPage(p5, _("File Types"), false);

	CompilerAdvancePage *p6 = new CompilerAdvancePage(m_compilersNotebook, cmp->GetName());
	pages.push_back(p6);
	m_compilersNotebook->AddSubPage(p6, _("Advanced"), false);

	m_compilerPagesMap[cmp->GetName()] = pages;
}

bool AdvancedDlg::DeleteCompiler ( const wxString &name )
{
	if ( wxMessageBox ( _ ( "Remove Compiler?" ), wxT ( "Confirm" ), wxYES_NO | wxICON_QUESTION ) == wxYES ) {
		BuildSettingsConfigST::Get()->DeleteCompiler ( name );
		return true;
	}
	return false;
}

void AdvancedDlg::OnContextMenu(wxContextMenuEvent& e)
{
	wxTreeCtrl *tree = m_compilersNotebook->GetTreeCtrl();
	wxTreeItemId item = tree->GetSelection();

	// only compilers have children
	if(item.IsOk() && tree->HasChildren(item)){
		PopupMenu(m_rightclickMenu);
	}
}
