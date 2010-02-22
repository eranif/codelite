///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newprojectbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewProjectBaseDlg::NewProjectBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 600,350 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* templatesSizer;
	templatesSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* categoriesSizer;
	categoriesSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Categories:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	categoriesSizer->Add( m_staticText2, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_chCategoriesChoices[] = { _("Console"), _("GUI"), _("Library"), _("Others"), _("User templates"), _("All") };
	int m_chCategoriesNChoices = sizeof( m_chCategoriesChoices ) / sizeof( wxString );
	m_chCategories = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 275,-1 ), m_chCategoriesNChoices, m_chCategoriesChoices, 0 );
	m_chCategories->SetSelection( 0 );
	m_chCategories->SetMinSize( wxSize( 275,-1 ) );
	
	categoriesSizer->Add( m_chCategories, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Templates:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	categoriesSizer->Add( m_staticText3, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_listTemplates = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxLC_REPORT|wxLC_SINGLE_SEL );
	m_listTemplates->SetMinSize( wxSize( -1,200 ) );
	
	categoriesSizer->Add( m_listTemplates, 1, wxALL|wxEXPAND, 5 );
	
	templatesSizer->Add( categoriesSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* detailsSizer;
	detailsSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Project name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	detailsSizer->Add( m_staticText4, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_txtProjName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 365,-1 ), 0 );
	detailsSizer->Add( m_txtProjName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Project path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	detailsSizer->Add( m_staticText5, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlProjectPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_textCtrlProjectPath, 1, wxALL|wxEXPAND, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button1, 0, wxALL, 5 );
	
	detailsSizer->Add( bSizer5, 0, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Compiler type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	detailsSizer->Add( m_staticText6, 0, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString m_chCompilerChoices;
	m_chCompiler = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_chCompilerChoices, 0 );
	m_chCompiler->SetSelection( 0 );
	detailsSizer->Add( m_chCompiler, 0, wxALL|wxEXPAND, 5 );
	
	m_cbSeparateDir = new wxCheckBox( this, wxID_ANY, _("Create the project under a separate directory"), wxDefaultPosition, wxDefaultSize, 0 );
	detailsSizer->Add( m_cbSeparateDir, 0, wxALL, 5 );
	
	wxStaticBoxSizer* labelSizer;
	labelSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("File name:") ), wxVERTICAL );
	
	m_stxtFullFileName = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_stxtFullFileName->Wrap( -1 );
	labelSizer->Add( m_stxtFullFileName, 0, wxALL|wxEXPAND, 5 );
	
	detailsSizer->Add( labelSizer, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_txtDescription = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_txtDescription->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_txtDescription->SetMinSize( wxSize( -1,60 ) );
	
	detailsSizer->Add( m_txtDescription, 1, wxALL|wxEXPAND, 5 );
	
	templatesSizer->Add( detailsSizer, 1, wxEXPAND, 5 );
	
	mainSizer->Add( templatesSizer, 1, wxEXPAND, 5 );
	
	m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetDefault(); 
	bSizer6->Add( m_button2, 0, wxALL, 5 );
	
	m_button3 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_button3, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_chCategories->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewProjectBaseDlg::OnCategorySelected ), NULL, this );
	m_listTemplates->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( NewProjectBaseDlg::OnTemplateSelected ), NULL, this );
	m_txtProjName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectNameChanged ), NULL, this );
	m_textCtrlProjectPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnBrowseProjectPath ), NULL, this );
	m_cbSeparateDir->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectNameChanged ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnCreate ), NULL, this );
}

NewProjectBaseDlg::~NewProjectBaseDlg()
{
	// Disconnect Events
	m_chCategories->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NewProjectBaseDlg::OnCategorySelected ), NULL, this );
	m_listTemplates->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( NewProjectBaseDlg::OnTemplateSelected ), NULL, this );
	m_txtProjName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectNameChanged ), NULL, this );
	m_textCtrlProjectPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectPathUpdated ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnBrowseProjectPath ), NULL, this );
	m_cbSeparateDir->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnProjectNameChanged ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewProjectBaseDlg::OnCreate ), NULL, this );
}
