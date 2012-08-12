///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
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
	m_panelEnv = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panelEnv, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText3 = new wxStaticText( m_panelEnv, wxID_ANY, _("By default, CodeLite uses the current active environment variables set as defined in the Settings > Environment Variables dialog.\nHowever, you may choose a different set to become the active set when this workspace is loaded selecting it here."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	sbSizer3->Add( m_staticText3, 0, wxALL, 5 );
	
	bSizer81->Add( sbSizer3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panelEnv, wxID_ANY, _("Environment sets:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer81->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_choiceEnvSetsChoices[] = { _("Default") };
	int m_choiceEnvSetsNChoices = sizeof( m_choiceEnvSetsChoices ) / sizeof( wxString );
	m_choiceEnvSets = new wxChoice( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEnvSetsNChoices, m_choiceEnvSetsChoices, 0 );
	m_choiceEnvSets->SetSelection( 0 );
	bSizer81->Add( m_choiceEnvSets, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer81->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText6 = new wxStaticText( m_panelEnv, wxID_ANY, _("Specify here an additional environment variables that will be shared with other people who are using this workspace:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer81->Add( m_staticText6, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlWspEnvVars = new wxTextCtrl( m_panelEnv, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlWspEnvVars->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer81->Add( m_textCtrlWspEnvVars, 1, wxALL|wxEXPAND, 5 );
	
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
	m_choiceEnvSets->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WorkspaceSettingsBase::OnEnvSelected ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnButtonOK ), NULL, this );
}

WorkspaceSettingsBase::~WorkspaceSettingsBase()
{
	// Disconnect Events
	m_choiceEnvSets->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( WorkspaceSettingsBase::OnEnvSelected ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( WorkspaceSettingsBase::OnButtonOK ), NULL, this );
	
}

CodeCompletionBasePage::CodeCompletionBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter1->SetSashGravity( 0 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( CodeCompletionBasePage::m_splitter1OnIdle ), NULL, this );
	m_splitter1->SetMinimumPaneSize( 1 );
	
	m_panel8 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText5 = new wxStaticText( m_panel8, wxID_ANY, _("Search paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer24->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_textCtrlSearchPaths = new wxTextCtrl( m_panel8, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlSearchPaths->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlSearchPaths->SetToolTip( _("Add here search paths used by clang / ctags for locating include files") );
	
	bSizer24->Add( m_textCtrlSearchPaths, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_panel8->SetSizer( bSizer24 );
	m_panel8->Layout();
	bSizer24->Fit( m_panel8 );
	m_panel6 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter3 = new wxSplitterWindow( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter3->SetSashGravity( 0.5 );
	m_splitter3->Connect( wxEVT_IDLE, wxIdleEventHandler( CodeCompletionBasePage::m_splitter3OnIdle ), NULL, this );
	m_splitter3->SetMinimumPaneSize( 1 );
	
	m_panel9 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter31 = new wxSplitterWindow( m_panel9, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME );
	m_splitter31->Connect( wxEVT_IDLE, wxIdleEventHandler( CodeCompletionBasePage::m_splitter31OnIdle ), NULL, this );
	
	m_panel61 = new wxPanel( m_splitter31, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( m_panel61, wxID_ANY, _("C++ Compile flags (clang only):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer91->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_textCtrlCmpOptions = new wxTextCtrl( m_panel61, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlCmpOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlCmpOptions->SetToolTip( _("Add here options to be passed to clang when generating PCH file.\nbackticks expressions are allowed (e.g. `wx-config --cflags`)") );
	
	bSizer91->Add( m_textCtrlCmpOptions, 1, wxEXPAND|wxLEFT, 5 );
	
	m_panel61->SetSizer( bSizer91 );
	m_panel61->Layout();
	bSizer91->Fit( m_panel61 );
	m_panel7 = new wxPanel( m_splitter31, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText7 = new wxStaticText( m_panel7, wxID_ANY, _("C Compile flags (clang only):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer10->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_textCtrlCmpOptionsC = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlCmpOptionsC->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer10->Add( m_textCtrlCmpOptionsC, 1, wxEXPAND|wxRIGHT, 5 );
	
	m_panel7->SetSizer( bSizer10 );
	m_panel7->Layout();
	bSizer10->Fit( m_panel7 );
	m_splitter31->SplitVertically( m_panel61, m_panel7, 0 );
	bSizer22->Add( m_splitter31, 1, wxEXPAND, 5 );
	
	m_panel9->SetSizer( bSizer22 );
	m_panel9->Layout();
	bSizer22->Fit( m_panel9 );
	m_panel10 = new wxPanel( m_splitter3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText12 = new wxStaticText( m_panel10, wxID_ANY, _("Macros (clang only):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer23->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_textCtrlMacros = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlMacros->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlMacros->SetToolTip( _("Add here macros to pass to clang when generating PCH files\nOne macro per line") );
	
	bSizer23->Add( m_textCtrlMacros, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_panel10->SetSizer( bSizer23 );
	m_panel10->Layout();
	bSizer23->Fit( m_panel10 );
	m_splitter3->SplitHorizontally( m_panel9, m_panel10, 0 );
	bSizer221->Add( m_splitter3, 1, wxEXPAND, 5 );
	
	m_panel6->SetSizer( bSizer221 );
	m_panel6->Layout();
	bSizer221->Fit( m_panel6 );
	m_splitter1->SplitHorizontally( m_panel8, m_panel6, 0 );
	bSizer9->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer9 );
	this->Layout();
	
	// Connect Events
	m_textCtrlSearchPaths->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlSearchPaths->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeCompletionBasePage::OnClangCCEnabledUI ), NULL, this );
	m_textCtrlCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlCmpOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeCompletionBasePage::OnClangCCEnabledUI ), NULL, this );
	m_textCtrlCmpOptionsC->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlMacros->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
}

CodeCompletionBasePage::~CodeCompletionBasePage()
{
	// Disconnect Events
	m_textCtrlSearchPaths->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlSearchPaths->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeCompletionBasePage::OnClangCCEnabledUI ), NULL, this );
	m_textCtrlCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlCmpOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CodeCompletionBasePage::OnClangCCEnabledUI ), NULL, this );
	m_textCtrlCmpOptionsC->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	m_textCtrlMacros->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( CodeCompletionBasePage::OnCCContentModified ), NULL, this );
	
}
