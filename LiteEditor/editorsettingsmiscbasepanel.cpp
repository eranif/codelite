///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsmiscbasepanel.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsMiscBasePanel::EditorSettingsMiscBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxString m_toolbarIconSizeChoices[] = { wxT("Toolbar uses small icons (16x16)"), wxT("Toolbar uses large icons (24x24)") };
	int m_toolbarIconSizeNChoices = sizeof( m_toolbarIconSizeChoices ) / sizeof( wxString );
	m_toolbarIconSize = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_toolbarIconSizeNChoices, m_toolbarIconSizeChoices, 0 );
	m_toolbarIconSize->SetSelection( 1 );
	bSizer1->Add( m_toolbarIconSize, 0, wxALL|wxEXPAND, 5 );
	
	m_useSingleToolbar = new wxCheckBox( this, wxID_ANY, wxT("Use single toolbar"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_useSingleToolbar, 0, wxALL, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("File font encoding"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString m_fileEncodingChoices;
	m_fileEncoding = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_fileEncodingChoices, 0 );
	m_fileEncoding->SetSelection( 0 );
	fgSizer1->Add( m_fileEncoding, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_showSplashScreen = new wxCheckBox( this, wxID_ANY, wxT("Show splashscreen on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showSplashScreen->SetValue(true); 
	bSizer1->Add( m_showSplashScreen, 0, wxALL, 5 );
	
	m_singleAppInstance = new wxCheckBox( this, wxID_ANY, wxT("Allow only single instance running"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_singleAppInstance, 0, wxALL, 5 );
	
	m_versionCheckOnStartup = new wxCheckBox( this, wxID_ANY, wxT("Check for new version on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_versionCheckOnStartup->SetValue(true); 
	bSizer1->Add( m_versionCheckOnStartup, 0, wxALL, 5 );
	
	m_fullFilePath = new wxCheckBox( this, wxID_ANY, wxT("Show file's full path in frame title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fullFilePath->SetValue(true); 
	bSizer1->Add( m_fullFilePath, 0, wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Clear recent workspace / files history"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_clearButton = new wxButton( this, wxID_ANY, wxT("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_clearButton, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Max items kept in find / replace dialog:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_maxItemsFindReplace = new wxSpinCtrl( this, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 50, 10 );
	fgSizer2->Add( m_maxItemsFindReplace, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Maximum number of tabs opened in the editor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlMaxOpenTabs = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 5, 30, 15 );
	fgSizer2->Add( m_spinCtrlMaxOpenTabs, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_clearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnClearButtonClick ), NULL, this );
	m_clearButton->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnClearUI ), NULL, this );
}

EditorSettingsMiscBasePanel::~EditorSettingsMiscBasePanel()
{
	// Disconnect Events
	m_clearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnClearButtonClick ), NULL, this );
	m_clearButton->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnClearUI ), NULL, this );
}
