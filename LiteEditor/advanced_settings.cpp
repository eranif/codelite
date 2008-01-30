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

AdvancedDlg::AdvancedDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style | wxRESIZE_BORDER )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxTreebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_notebook->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
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
	
	m_compilersNotebook = new wxFlatNotebook(m_compilersPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFNB_BACKGROUND_GRADIENT|wxFNB_DROPDOWN_TABS_LIST|wxFNB_NO_NAV_BUTTONS|wxFNB_NO_X_BUTTON|wxFNB_FF2);
	m_compilersNotebook->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
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

	m_compilersNotebook->SetRightClickMenu(wxXmlResource::Get()->LoadMenu(wxT("delete_compiler_menu")));

	LoadCompilers();
	ConnectButton(m_buttonNewCompiler, AdvancedDlg::OnButtonNewClicked);
	ConnectButton(m_buttonOK, AdvancedDlg::OnButtonOKClicked);
}

void AdvancedDlg::LoadCompilers()
{
	m_compilersNotebook->Freeze();
	m_compilersNotebook->DeleteAllPages();
	
	BuildSettingsConfigCookie cookie;
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
	while(cmp){
		m_compilersNotebook->AddPage(new CompilerPage(m_compilersNotebook, cmp->GetName()), cmp->GetName()); 
		cmp = BuildSettingsConfigST::Get()->GetNextCompiler(cookie);
	}
	m_compilersNotebook->Thaw();
}

void AdvancedDlg::OnButtonNewClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("Enter New Compiler Name:"), wxT("New Compiler"));
	if(dlg->ShowModal() == wxID_OK){
		wxString name = dlg->GetValue();
		TrimString(name);
		if(name.IsEmpty() == false){
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
	for(int i=0; i<count; i++){
		CompilerPage *page = dynamic_cast<CompilerPage*>( m_compilersNotebook->GetPage((size_t)i));
		if(page){
			page->Save();
		}
	}
	//save the build page
	m_buildPage->Save();
	m_buildSettings->Save();
	
	//mark all the projects as dirty
	wxArrayString projects;
	WorkspaceST::Get()->GetProjectList( projects );
	for( size_t i=0; i< projects.size(); i++ ) {
		ProjectPtr proj = ManagerST::Get()->GetProject( projects.Item(0) );
		if( proj ) {
			proj->SetModified( true );
		}
	}
	EndModal(wxID_OK);
}

void AdvancedDlg::OnDeleteCompiler(wxCommandEvent & event)
{
	wxUnusedVar(event);
	int sel = m_compilersNotebook->GetSelection();
	
	if(sel != wxNOT_FOUND){
		wxString name = m_compilersNotebook->GetPageText((size_t)sel);
		if(ManagerST::Get()->DeleteCompiler(name)){
			m_compilersNotebook->DeletePage((size_t)sel);
		}
	}
}
