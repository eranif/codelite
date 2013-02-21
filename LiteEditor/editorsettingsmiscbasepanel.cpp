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
	
	m_auinotebook1 = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_SCROLL_BUTTONS|wxNO_BORDER );
	m_panel1 = new wxPanel( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText7 = new wxStaticText( m_panel1, wxID_ANY, _("Icon Set:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer4->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceIconSetChoices[] = { _("Classic"), _("Fresh Farm"), _("Classic - Dark") };
	int m_choiceIconSetNChoices = sizeof( m_choiceIconSetChoices ) / sizeof( wxString );
	m_choiceIconSet = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceIconSetNChoices, m_choiceIconSetChoices, 0 );
	m_choiceIconSet->SetSelection( 1 );
	fgSizer4->Add( m_choiceIconSet, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_staticText8 = new wxStaticText( m_panel1, wxID_ANY, _("Toolbar Icon Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer4->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_toolbarIconSizeChoices[] = { _("Small Icons (16x16)"), _("Large Icons (24x24)") };
	int m_toolbarIconSizeNChoices = sizeof( m_toolbarIconSizeChoices ) / sizeof( wxString );
	m_toolbarIconSize = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_toolbarIconSizeNChoices, m_toolbarIconSizeChoices, 0 );
	m_toolbarIconSize->SetSelection( 1 );
	fgSizer4->Add( m_toolbarIconSize, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer8->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_useSingleToolbar = new wxCheckBox( m_panel1, wxID_ANY, _("Use single toolbar"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_useSingleToolbar, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxEnableMSWTheme = new wxCheckBox( m_panel1, wxID_ANY, _("Enable Windows(R) theme for Vista / Windows 7"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_checkBoxEnableMSWTheme, 0, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer8, 0, wxEXPAND|wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_showSplashScreen = new wxCheckBox( m_panel1, wxID_ANY, _("Show splashscreen on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_showSplashScreen->SetValue(true); 
	bSizer6->Add( m_showSplashScreen, 0, wxALL, 5 );
	
	m_singleAppInstance = new wxCheckBox( m_panel1, wxID_ANY, _("Allow only single instance running"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_singleAppInstance, 0, wxALL, 5 );
	
	m_versionCheckOnStartup = new wxCheckBox( m_panel1, wxID_ANY, _("Check for new version on startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_versionCheckOnStartup->SetValue(true); 
	bSizer6->Add( m_versionCheckOnStartup, 0, wxALL, 5 );
	
	m_fullFilePath = new wxCheckBox( m_panel1, wxID_ANY, _("Show file's full path in frame title"), wxDefaultPosition, wxDefaultSize, 0 );
	m_fullFilePath->SetValue(true); 
	bSizer6->Add( m_fullFilePath, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer6, 0, wxEXPAND|wxALL, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_auinotebook1->AddPage( m_panel1, _("General"), false, wxNullBitmap );
	m_panel2 = new wxPanel( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("File font encoding:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxArrayString m_fileEncodingChoices;
	m_fileEncoding = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_fileEncodingChoices, 0 );
	m_fileEncoding->SetSelection( 0 );
	fgSizer1->Add( m_fileEncoding, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_SetLocale = new wxCheckBox( m_panel2, wxID_ANY, _("Enable localization"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SetLocale->SetValue(true); 
	m_SetLocale->SetToolTip( _("View CodeLite's strings translated into a different language, if available.  This will also make CodeLite use other aspects of the locale.") );
	
	fgSizer1->Add( m_SetLocale, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticTextLocale = new wxStaticText( m_panel2, wxID_ANY, _("Locale to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextLocale->Wrap( -1 );
	fgSizer1->Add( m_staticTextLocale, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_AvailableLocalesChoices;
	m_AvailableLocales = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_AvailableLocalesChoices, 0 );
	m_AvailableLocales->SetSelection( 0 );
	m_AvailableLocales->SetToolTip( _("These are the locales that are available on your system. There won't necessarily be CodeLite translations for all of them.") );
	
	fgSizer1->Add( m_AvailableLocales, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer4->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_auinotebook1->AddPage( m_panel2, _("Encoding & Locale"), false, wxNullBitmap );
	m_panel3 = new wxPanel( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText2 = new wxStaticText( m_panel3, wxID_ANY, _("Clear recent workspace / files history"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_clearButton = new wxButton( m_panel3, wxID_CLEAR, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_clearButton->SetDefault(); 
	fgSizer2->Add( m_clearButton, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_staticText3 = new wxStaticText( m_panel3, wxID_ANY, _("Max items kept in find / replace dialog:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_maxItemsFindReplace = new wxTextCtrl( m_panel3, wxID_ANY, _("10"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_maxItemsFindReplace, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText4 = new wxStaticText( m_panel3, wxID_ANY, _("Maximum number of tabs opened in the editor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlMaxOpenTabs = new wxTextCtrl( m_panel3, wxID_ANY, _("15"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_spinCtrlMaxOpenTabs, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_panel3->SetSizer( fgSizer2 );
	m_panel3->Layout();
	fgSizer2->Fit( m_panel3 );
	m_auinotebook1->AddPage( m_panel3, _("History"), true, wxNullBitmap );
	m_panel4 = new wxPanel( m_auinotebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_panel4, wxID_ANY, _("Log file verbosity:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer3->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choice4Choices[] = { _("Error"), _("Warning"), _("Debug"), _("Developer") };
	int m_choice4NChoices = sizeof( m_choice4Choices ) / sizeof( wxString );
	m_choice4 = new wxChoice( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice4NChoices, m_choice4Choices, 0 );
	m_choice4->SetSelection( 0 );
	m_choice4->SetToolTip( _("codelite logs to file various events, this option controls the logging verbosity") );
	
	fgSizer3->Add( m_choice4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_buttonOpenLog = new wxButton( m_panel4, wxID_ANY, _("Open.."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOpenLog->SetToolTip( _("Open the log file into an editor") );
	
	fgSizer3->Add( m_buttonOpenLog, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALIGN_RIGHT, 5 );
	
	m_panel4->SetSizer( fgSizer3 );
	m_panel4->Layout();
	fgSizer3->Fit( m_panel4 );
	m_auinotebook1->AddPage( m_panel4, _("Log"), false, wxNullBitmap );
	
	bSizer1->Add( m_auinotebook1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_checkBoxEnableMSWTheme->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnEnableThemeUI ), NULL, this );
	m_SetLocale->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChkUpdateUI ), NULL, this );
	m_staticTextLocale->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleStaticUpdateUI ), NULL, this );
	m_AvailableLocales->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::LocaleChoiceUpdateUI ), NULL, this );
	m_clearButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnClearButtonClick ), NULL, this );
	m_clearButton->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsMiscBasePanel::OnClearUI ), NULL, this );
	m_choice4->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnLogVerbosityChanged ), NULL, this );
	m_buttonOpenLog->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnShowLogFile ), NULL, this );
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
	m_choice4->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnLogVerbosityChanged ), NULL, this );
	m_buttonOpenLog->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( EditorSettingsMiscBasePanel::OnShowLogFile ), NULL, this );
	
}
