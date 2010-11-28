///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "workspacesettingsbase.h"

///////////////////////////////////////////////////////////////////////////

WorkspaceSettingsBase::WorkspaceSettingsBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Add search path(s) for the code completion parser.\nThe search paths are used for locating include files for this workspace ONLY"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxSearchPaths = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer5->Add( m_listBoxSearchPaths, 1, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_button3 = new wxButton( m_panel2, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button3, 0, wxALL|wxEXPAND, 5 );
	
	m_button4 = new wxButton( m_panel2, wxID_ANY, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button4, 0, wxALL|wxEXPAND, 5 );
	
	m_button5 = new wxButton( m_panel2, wxID_ANY, _("Clear All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button5, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( bSizer7, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel2, wxID_ANY, _("Add exclude path(s) for the code completion parser.\nCodeLite will skip any file found inside these paths while scanning for include files for this workspace ONLY"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	sbSizer2->Add( m_staticText2, 1, wxEXPAND|wxALL, 5 );
	
	bSizer4->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listBoxExcludePaths = new wxListBox( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer6->Add( m_listBoxExcludePaths, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_button6 = new wxButton( m_panel2, wxID_ANY, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( m_panel2, wxID_ANY, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button7, 0, wxALL, 5 );
	
	m_button8 = new wxButton( m_panel2, wxID_ANY, _("Clear All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button8, 0, wxALL, 5 );
	
	bSizer6->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer6, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("Parser Include Files"), true );
	m_panelEnv = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panelEnv, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_panelEnv, wxID_ANY, _("By default, CodeLite uses the current active environment variables set as defined in the Settings > Environment Variables dialog.\nHowever, you may choose a different set to become the active set when this workspace is loaded selecting it here."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer3->Add( m_staticText3, 0, wxALL|wxEXPAND, 5 );
	
	bSizer81->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panelEnv, wxID_ANY, _("Environment sets:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer81->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_choiceEnvSetsChoices[] = { _("Default") };
	int m_choiceEnvSetsNChoices = sizeof( m_choiceEnvSetsChoices ) / sizeof( wxString );
	m_choiceEnvSets = new wxChoice( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEnvSetsNChoices, m_choiceEnvSetsChoices, 0 );
	m_choiceEnvSets->SetSelection( 0 );
	bSizer81->Add( m_choiceEnvSets, 0, wxALL|wxEXPAND, 5 );
	
	m_panelEnv->SetSizer( bSizer81 );
	m_panelEnv->Layout();
	bSizer81->Fit( m_panelEnv );
	m_notebook1->AddPage( m_panelEnv, _("Environment"), false );
	
	mainSizer->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnAddIncludePath ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnRemoveIncludePath ), NULL, this );
	m_button4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnRemoveIncludePathUI ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnClearAllIncludePaths ), NULL, this );
	m_button5->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnClearAllIncludePathUI ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnAddExcludePath ), NULL, this );
	m_button7->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnRemoveExcludePath ), NULL, this );
	m_button7->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnRemoveExcludePathUI ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnClearAllExcludePaths ), NULL, this );
	m_button8->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnClearAllExcludePathsUI ), NULL, this );
	m_choiceEnvSets->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WorkspaceSettingsBase::OnEnvSelected ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnButtonOK ), NULL, this );
}

WorkspaceSettingsBase::~WorkspaceSettingsBase()
{
	// Disconnect Events
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnAddIncludePath ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnRemoveIncludePath ), NULL, this );
	m_button4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnRemoveIncludePathUI ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnClearAllIncludePaths ), NULL, this );
	m_button5->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnClearAllIncludePathUI ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnAddExcludePath ), NULL, this );
	m_button7->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnRemoveExcludePath ), NULL, this );
	m_button7->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnRemoveExcludePathUI ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnClearAllExcludePaths ), NULL, this );
	m_button8->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WorkspaceSettingsBase::OnClearAllExcludePathsUI ), NULL, this );
	m_choiceEnvSets->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WorkspaceSettingsBase::OnEnvSelected ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnButtonOK ), NULL, this );
	
}
