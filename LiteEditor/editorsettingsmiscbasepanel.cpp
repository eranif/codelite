///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Toolbar and Theme:") ), wxVERTICAL );
	
	wxString m_toolbarIconSizeChoices[] = { _("Toolbar uses small icons (16x16)"), _("Toolbar uses large icons (24x24)") };
	int m_toolbarIconSizeNChoices = sizeof( m_toolbarIconSizeChoices ) / sizeof( wxString );
	m_toolbarIconSize = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_toolbarIconSizeNChoices, m_toolbarIconSizeChoices, 0 );
	m_toolbarIconSize->SetSelection( 1 );
	sbSizer1->Add( m_toolbarIconSize, 0, wxALL|wxEXPAND, 5 );
	
	m_useSingleToolbar = new wxCheckBox( this, wxID_ANY, _("Use single toolbar"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_useSingleToolbar, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxEnableMSWTheme = new wxCheckBox( this, wxID_ANY, _("Enable Windows(R) theme for Vista / Windows 7"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxEnableMSWTheme, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Encoding and Locale:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("File font encoding:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString m_fileEncodingChoices;
	m_fileEncoding = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_fileEncodingChoices, 0 );
	m_fileEncoding->SetSelection( 0 );
	fgSizer1->Add( m_fileEncoding, 0, wxALL|wxEXPAND, 5 );
	
	m_SetLocale = new wxCheckBox( this, wxID_ANY, _("Enable localization"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SetLocale->SetValue(true); 
	m_SetLocale->SetToolTip( _("View CodeLite's strings translated into a different language, if available.  This will also make CodeLite use other aspects of the locale.") );
	
	fgSizer1->Add( m_SetLocale, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextLocale = new wxStaticText( this, wxID_ANY, _("Locale to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLocale->Wrap( -1 );
	fgSizer1->Add( m_staticTextLocale, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_AvailableLocalesChoices;
	m_AvailableLocales = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AvailableLocalesChoices, 0 );
	m_AvailableLocales->SetSelection( 0 );
	m_AvailableLocales->SetToolTip( _("These are the locales that are available on your system. There won't necessarily be CodeLite translations for all of them.") );
	
	fgSizer1->Add( m_AvailableLocales, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer2->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("History:") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Clear recent workspace / files history"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_clearButton = new wxButton( this, wxID_ANY, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_clearButton, 0, wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Max items kept in find / replace dialog:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_maxItemsFindReplace = new wxSpinCtrl( this, wxID_ANY, wxT("10"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 50, 10 );
	fgSizer2->Add( m_maxItemsFindReplace, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Maximum number of tabs opened in the editor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlMaxOpenTabs = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 5, 30, 15 );
	fgSizer2->Add( m_spinCtrlMaxOpenTabs, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	sbSizer4->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	bSizer1->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Others:") ), wxVERTICAL );
	
	m_showSplashScreen = new wxCheckBox( this, wxID_ANY, _("Show splashscreen on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showSplashScreen->SetValue(true); 
	sbSizer3->Add( m_showSplashScreen, 0, wxALL, 5 );
	
	m_singleAppInstance = new wxCheckBox( this, wxID_ANY, _("Allow only single instance running"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( m_singleAppInstance, 0, wxALL, 5 );
	
	m_versionCheckOnStartup = new wxCheckBox( this, wxID_ANY, _("Check for new version on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_versionCheckOnStartup->SetValue(true); 
	sbSizer3->Add( m_versionCheckOnStartup, 0, wxALL, 5 );
	
	m_fullFilePath = new wxCheckBox( this, wxID_ANY, _("Show file's full path in frame title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fullFilePath->SetValue(true); 
	sbSizer3->Add( m_fullFilePath, 0, wxALL, 5 );
	
	bSizer1->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	m_checkBoxEnableMSWTheme->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnEnableThemeUI ), NULL, this );
	m_SetLocale->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChkUpdateUI ), NULL, this );
	m_staticTextLocale->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleStaticUpdateUI ), NULL, this );
	m_AvailableLocales->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChoiceUpdateUI ), NULL, this );
	m_clearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnClearButtonClick ), NULL, this );
	m_clearButton->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnClearUI ), NULL, this );
}

EditorSettingsMiscBasePanel::~EditorSettingsMiscBasePanel()
{
	// Disconnect Events
	m_checkBoxEnableMSWTheme->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnEnableThemeUI ), NULL, this );
	m_SetLocale->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChkUpdateUI ), NULL, this );
	m_staticTextLocale->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleStaticUpdateUI ), NULL, this );
	m_AvailableLocales->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChoiceUpdateUI ), NULL, this );
	m_clearButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnClearButtonClick ), NULL, this );
	m_clearButton->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnClearUI ), NULL, this );
	
}
