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

#include "build_page.h"
#include "build_settings_config.h"
#include "buildmanager.h"

///////////////////////////////////////////////////////////////////////////

BuildPage::BuildPage( wxWindow* parent, int id, wxPoint pos, wxSize size, int style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText = new wxStaticText( this, wxID_ANY, wxT("Available Build Systems:"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_staticText, 0, wxALL, 5 );
	
	m_bookBuildSystems = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT );
	mainSizer->Add( m_bookBuildSystems, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
    CustomInit();
}

void BuildPage::CustomInit()
{
	std::list<wxString> builders;
	BuildManagerST::Get()->GetBuilders(builders);

	std::list<wxString>::iterator iter = builders.begin();
	for(; iter != builders.end(); iter++){
		m_bookBuildSystems->AddPage(CreateBuildSystemPage(*iter), *iter);
	}
}

wxPanel *BuildPage::CreateBuildSystemPage(const wxString &name)
{
	return new BuildSystemPage(m_bookBuildSystems, name);
}

void BuildPage::Save()
{
	int count = (int)m_bookBuildSystems->GetPageCount();
	for(int i=0; i<count; i++){
		BuildSystemPage *page = dynamic_cast<BuildSystemPage*>(m_bookBuildSystems->GetPage(i));
		if(page){
			page->Save();
		}
	}
}

//---------------------------------------------------------------
// Build system page
//---------------------------------------------------------------
BuildSystemPage::BuildSystemPage(wxWindow *parent, wxString name)
: wxPanel(parent)
, m_name(name)
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("Build Tool:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_filePicker = new FilePicker(this);
	fgSizer4->Add( m_filePicker, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("Build Tool Switches:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textBuildToolOptions = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textBuildToolOptions, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer6 );
	this->Layout();

	//set the default build tool path
	m_filePicker->SetPath(BuildManagerST::Get()->GetBuilder(name)->GetBuildToolName());
	m_textBuildToolOptions->SetValue(BuildManagerST::Get()->GetBuilder(name)->GetBuildToolOptions());
}

void BuildSystemPage::Save()
{
	//update cached builders
	BuilderPtr builder = BuildManagerST::Get()->GetBuilder(m_name);
	builder->SetBuildTool(m_filePicker->GetPath());
	builder->SetBuildToolOptions(m_textBuildToolOptions->GetValue());
	BuildManagerST::Get()->AddBuilder(builder);

	//update configuration file
	BuildSystemPtr bsptr = BuildSettingsConfigST::Get()->GetBuildSystem(m_name);
	if(!bsptr){
		bsptr = new BuildSystem(NULL);
		bsptr->SetName(m_name);
		
	}
	bsptr->SetToolPath(m_filePicker->GetPath());	
	bsptr->SetToolOptions(m_textBuildToolOptions->GetValue());
	BuildSettingsConfigST::Get()->SetBuildSystem(bsptr);
}
