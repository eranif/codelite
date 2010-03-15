///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
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

#include "project_settings_base_dlg.h"

///////////////////////////////////////////////////////////////////////////

ProjectSettingsBaseDlg::ProjectSettingsBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("Configuration Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer22->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceConfigurationTypeChoices;
	m_choiceConfigurationType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceConfigurationTypeChoices, 0 );
	m_choiceConfigurationType->SetSelection( 0 );
	bSizer22->Add( m_choiceConfigurationType, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonConfigManager = new wxButton( this, wxID_ANY, wxT("Configuration Manager..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_buttonConfigManager, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer22, 0, wxEXPAND, 5 );
	
	m_staticline81 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline81, 0, wxEXPAND | wxALL, 5 );
	
	m_panelSettings = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_sizerSettings = new wxBoxSizer( wxVERTICAL );
	
	m_panelSettings->SetSizer( m_sizerSettings );
	m_panelSettings->Layout();
	m_sizerSettings->Fit( m_panelSettings );
	mainSizer->Add( m_panelSettings, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonHelp = new wxButton( this, wxID_ANY, wxT("Help..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonHelp, 0, wxALL, 5 );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonApply, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_choiceConfigurationType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnConfigurationTypeSelected ), NULL, this );
	m_buttonConfigManager->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonConfigurationManager ), NULL, this );
	m_buttonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonApply ), NULL, this );
}

ProjectSettingsBaseDlg::~ProjectSettingsBaseDlg()
{
	// Disconnect Events
	m_choiceConfigurationType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnConfigurationTypeSelected ), NULL, this );
	m_buttonConfigManager->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonConfigurationManager ), NULL, this );
	m_buttonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonApply ), NULL, this );
}

ProjectConfigurationBasePanel::ProjectConfigurationBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_generalPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 5, 3, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText22 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Project Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer3->Add( m_staticText22, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceProjectTypesChoices[] = { wxT("Static Library"), wxT("Dynamic Library"), wxT("Executable") };
	int m_choiceProjectTypesNChoices = sizeof( m_choiceProjectTypesChoices ) / sizeof( wxString );
	m_choiceProjectTypes = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjectTypesNChoices, m_choiceProjectTypesChoices, 0 );
	m_choiceProjectTypes->SetSelection( 0 );
	fgSizer3->Add( m_choiceProjectTypes, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText191 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	fgSizer3->Add( m_staticText191, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCompilerTypeChoices;
	m_choiceCompilerType = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCompilerTypeChoices, 0 );
	m_choiceCompilerType->SetSelection( 0 );
	fgSizer3->Add( m_choiceCompilerType, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText231 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Debugger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	fgSizer3->Add( m_staticText231, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDebuggerChoices;
	m_choiceDebugger = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggerChoices, 0 );
	m_choiceDebugger->SetSelection( 0 );
	fgSizer3->Add( m_choiceDebugger, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Output File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer3->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textOutputFilePicker = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textOutputFilePicker, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer3->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Intermediate Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlItermediateDir = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlItermediateDir, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseIM_WD = new wxButton( m_generalPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_buttonBrowseIM_WD, 0, wxALL, 5 );
	
	bSizer19->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Program to Debug / Run:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer21->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline5 = new wxStaticLine( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer21->Add( m_staticline5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer19->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText18 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Program:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer6->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommand = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommand, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseProgram = new wxButton( m_generalPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_buttonBrowseProgram, 0, wxALL, 5 );
	
	m_staticText19 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Program Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer6->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommandArguments = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommandArguments, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizer6->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer6->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCommandWD = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlCommandWD, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseCommandWD = new wxButton( m_generalPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_buttonBrowseCommandWD, 0, wxALL, 5 );
	
	bSizer19->Add( fgSizer6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxPauseWhenExecEnds = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Pause when execution ends"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_checkBoxPauseWhenExecEnds, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( bSizer17, 1, wxEXPAND, 5 );
	
	m_generalPage->SetSizer( bSizer19 );
	m_generalPage->Layout();
	bSizer19->Fit( m_generalPage );
	m_notebook->AddPage( m_generalPage, wxT("General"), true );
	m_compilerPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* compilerPageSizer;
	compilerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkCompilerNeeded = new wxCheckBox( m_compilerPage, wxID_ANY, wxT("Compiler is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	compilerPageSizer->Add( m_checkCompilerNeeded, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer23001;
	bSizer23001 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText331 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Use with global settings :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331->Wrap( -1 );
	bSizer23001->Add( m_staticText331, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCmpUseWithGlobalSettingsChoices;
	m_choiceCmpUseWithGlobalSettings = new wxChoice( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCmpUseWithGlobalSettingsChoices, 0 );
	m_choiceCmpUseWithGlobalSettings->SetSelection( 0 );
	bSizer23001->Add( m_choiceCmpUseWithGlobalSettings, 1, wxALL, 5 );
	
	compilerPageSizer->Add( bSizer23001, 0, wxEXPAND, 5 );
	
	m_staticline7 = new wxStaticLine( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	compilerPageSizer->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer8->Add( m_staticText6, 0, wxALL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCompilerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_buttonCompilerOptions, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer8->Add( m_staticText4, 0, wxALL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textAdditionalSearchPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_buttonAddSearchPath, 0, wxALL, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer8->Add( m_staticText171, 0, wxALL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textPreprocessor, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_buttonAddPreprocessor, 0, wxALL, 5 );
	
	compilerPageSizer->Add( fgSizer8, 0, wxEXPAND, 5 );
	
	m_staticline14 = new wxStaticLine( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	compilerPageSizer->Add( m_staticline14, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_staticTextPreCompiledHeader = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Using pre-complied header via this header file:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextPreCompiledHeader->Wrap( -1 );
	bSizer32->Add( m_staticTextPreCompiledHeader, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlPreCompiledHeader = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_textCtrlPreCompiledHeader, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowsePreCompiledHeader = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer33->Add( m_buttonBrowsePreCompiledHeader, 0, wxALL, 5 );
	
	bSizer32->Add( bSizer33, 1, wxEXPAND, 5 );
	
	compilerPageSizer->Add( bSizer32, 0, wxEXPAND, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	m_notebook->AddPage( m_compilerPage, wxT("Compiler"), false );
	m_linkerPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkLinkerNeeded = new wxCheckBox( m_linkerPage, wxID_ANY, wxT("Linker is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	linkerPageSizer->Add( m_checkLinkerNeeded, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer231;
	bSizer231 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText3311 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Use with global settings :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3311->Wrap( -1 );
	bSizer231->Add( m_staticText3311, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceLnkUseWithGlobalSettingsChoices;
	m_choiceLnkUseWithGlobalSettings = new wxChoice( m_linkerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceLnkUseWithGlobalSettingsChoices, 0 );
	m_choiceLnkUseWithGlobalSettings->SetSelection( 0 );
	bSizer231->Add( m_choiceLnkUseWithGlobalSettings, 1, wxALL, 5 );
	
	linkerPageSizer->Add( bSizer231, 0, wxEXPAND, 5 );
	
	m_staticline8 = new wxStaticLine( m_linkerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	linkerPageSizer->Add( m_staticline8, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer9->AddGrowableCol( 1 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer9->Add( m_staticText10, 0, wxALL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_textLinkerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_buttonLinkerOptions, 0, wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Library Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer9->Add( m_staticText7, 0, wxALL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_textLibraryPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_buttonLibraryPath, 0, wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer9->Add( m_staticText8, 0, wxALL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_textLibraries, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_buttonLibraries, 0, wxALL, 5 );
	
	linkerPageSizer->Add( fgSizer9, 0, wxEXPAND, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	m_notebook->AddPage( m_linkerPage, wxT("Linker"), false );
	m_panelDebugger = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer192;
	bSizer192 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText321 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Select debugger path. Leave empty to use the default:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321->Wrap( -1 );
	bSizer192->Add( m_staticText321, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer62;
	fgSizer62 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer62->AddGrowableCol( 0 );
	fgSizer62->SetFlexibleDirection( wxBOTH );
	fgSizer62->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_textCtrlDebuggerPath = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlDebuggerPath->SetToolTip( wxT("You may override the global debugger executable path, by selecting another one here.\nLeave this field empty if you want to use the one set in the global debugger settings") );
	
	fgSizer62->Add( m_textCtrlDebuggerPath, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_buttonSelectDebugger = new wxButton( m_panelDebugger, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer62->Add( m_buttonSelectDebugger, 0, wxALL, 5 );
	
	bSizer192->Add( fgSizer62, 0, wxEXPAND, 5 );
	
	m_staticline121 = new wxStaticLine( m_panelDebugger, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer192->Add( m_staticline121, 0, wxEXPAND | wxALL, 5 );
	
	m_checkBoxDbgRemote = new wxCheckBox( m_panelDebugger, wxID_ANY, wxT("Debugging remote target"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer192->Add( m_checkBoxDbgRemote, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer61;
	fgSizer61 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer61->AddGrowableCol( 1 );
	fgSizer61->SetFlexibleDirection( wxBOTH );
	fgSizer61->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText31 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Host / tty:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer61->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl1DbgHost = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer61->Add( m_textCtrl1DbgHost, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText32 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer61->Add( m_staticText32, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlDbgPort = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer61->Add( m_textCtrlDbgPort, 0, wxALL, 5 );
	
	bSizer192->Add( fgSizer61, 0, wxRIGHT|wxLEFT|wxEXPAND, 15 );
	
	m_staticline131 = new wxStaticLine( m_panelDebugger, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer192->Add( m_staticline131, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText301 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Enter here any commands that should be passed to the debugger on startup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	bSizer192->Add( m_staticText301, 0, wxALL, 5 );
	
	m_textCtrlDbgCmds = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	bSizer192->Add( m_textCtrlDbgCmds, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText311 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Enter here any commands that should be passed to the debugger after attaching the remote target:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText311->Wrap( -1 );
	bSizer192->Add( m_staticText311, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlDbgPostConnectCmds = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	bSizer192->Add( m_textCtrlDbgPostConnectCmds, 1, wxALL|wxEXPAND, 5 );
	
	m_panelDebugger->SetSizer( bSizer192 );
	m_panelDebugger->Layout();
	bSizer192->Fit( m_panelDebugger );
	m_notebook->AddPage( m_panelDebugger, wxT("Debugger"), false );
	m_resourceCmpPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	m_checkResourceNeeded = new wxCheckBox( m_resourceCmpPage, wxID_ANY, wxT("Resource Compiler is not needed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkResourceNeeded->SetValue(true); 
	bSizer151->Add( m_checkResourceNeeded, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer2311;
	bSizer2311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText33111 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Use with global settings :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33111->Wrap( -1 );
	bSizer2311->Add( m_staticText33111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceResUseWithGlobalSettingsChoices;
	m_choiceResUseWithGlobalSettings = new wxChoice( m_resourceCmpPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceResUseWithGlobalSettingsChoices, 0 );
	m_choiceResUseWithGlobalSettings->SetSelection( 0 );
	bSizer2311->Add( m_choiceResUseWithGlobalSettings, 1, wxALL, 5 );
	
	bSizer151->Add( bSizer2311, 0, wxEXPAND, 5 );
	
	m_staticline9 = new wxStaticLine( m_resourceCmpPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer151->Add( m_staticline9, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText221 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer4->Add( m_staticText221, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpOptions = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpOptions, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpOptions = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpOptions, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText23 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer4->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpPath = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpPath, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpPath = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpPath, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer151->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	m_resourceCmpPage->SetSizer( bSizer151 );
	m_resourceCmpPage->Layout();
	bSizer151->Fit( m_resourceCmpPage );
	m_notebook->AddPage( m_resourceCmpPage, wxT("Resources"), false );
	m_preBuildPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( m_preBuildPage, wxID_ANY, wxT("Specifies here a command lines to run in the pre-build event:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer8->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_preBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListPreBuildCommandsChoices;
	m_checkListPreBuildCommands = new wxCheckListBox( m_preBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListPreBuildCommandsChoices, 0 );
	bSizer9->Add( m_checkListPreBuildCommands, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonNewPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonDeletePreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonDeletePreBuildCmd, 0, wxALL, 5 );
	
	m_buttonEditPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonEditPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonUpPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonUpPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonDownPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonDownPreBuildCmd, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	m_preBuildPage->SetSizer( bSizer8 );
	m_preBuildPage->Layout();
	bSizer8->Fit( m_preBuildPage );
	m_notebook->AddPage( m_preBuildPage, wxT("PreBuild"), false );
	m_postBuildPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText111 = new wxStaticText( m_postBuildPage, wxID_ANY, wxT("Specifies here a command lines to run in the post-build event:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer81->Add( m_staticText111, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticline21 = new wxStaticLine( m_postBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer81->Add( m_staticline21, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListPostBuildCommandsChoices;
	m_checkListPostBuildCommands = new wxCheckListBox( m_postBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListPostBuildCommandsChoices, 0 );
	bSizer91->Add( m_checkListPostBuildCommands, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonNewPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonDeletePostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonDeletePostBuildCmd, 0, wxALL, 5 );
	
	m_buttonEditPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonEditPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonUpPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonUpPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonDownPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonDownPostBuildCmd, 0, wxALL, 5 );
	
	bSizer91->Add( bSizer101, 0, wxEXPAND, 5 );
	
	bSizer81->Add( bSizer91, 1, wxEXPAND, 5 );
	
	m_postBuildPage->SetSizer( bSizer81 );
	m_postBuildPage->Layout();
	bSizer81->Fit( m_postBuildPage );
	m_notebook->AddPage( m_postBuildPage, wxT("PostBuild"), false );
	m_customBuildPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_checkEnableCustomBuild = new wxCheckBox( m_customBuildPage, wxID_ANY, wxT("Enable custom build"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_checkEnableCustomBuild, 0, wxALL, 5 );
	
	m_staticline12 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer15->Add( m_staticline12, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText33 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	bSizer23->Add( m_staticText33, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCustomBuildWD = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_textCtrlCustomBuildWD, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseCustomBuildWD = new wxButton( m_customBuildPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_buttonBrowseCustomBuildWD, 0, wxALL, 5 );
	
	bSizer15->Add( bSizer23, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer211;
	bSizer211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlTargets = new wxListCtrl( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer211->Add( m_listCtrlTargets, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline13 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer221->Add( m_staticline13, 0, wxEXPAND | wxALL, 5 );
	
	m_buttonNewCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonNewCustomTarget, 0, wxALL, 5 );
	
	m_buttonEditCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonEditCustomTarget, 0, wxALL, 5 );
	
	m_buttonDeleteCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonDeleteCustomTarget, 0, wxALL, 5 );
	
	bSizer211->Add( bSizer221, 0, wxEXPAND, 5 );
	
	bSizer15->Add( bSizer211, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText29 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Makefile Generators:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer191->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline11 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer191->Add( m_staticline11, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( bSizer191, 0, 0, 5 );
	
	wxString m_thirdPartyToolChoices[] = { wxT("None"), wxT("Other") };
	int m_thirdPartyToolNChoices = sizeof( m_thirdPartyToolChoices ) / sizeof( wxString );
	m_thirdPartyTool = new wxChoice( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_thirdPartyToolNChoices, m_thirdPartyToolChoices, 0 );
	m_thirdPartyTool->SetSelection( 0 );
	bSizer15->Add( m_thirdPartyTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText30 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Command to use for makefile generation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer15->Add( m_staticText30, 0, wxALL, 5 );
	
	m_textCtrlMakefileGenerationCmd = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_textCtrlMakefileGenerationCmd, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_customBuildPage->SetSizer( bSizer15 );
	m_customBuildPage->Layout();
	bSizer15->Fit( m_customBuildPage );
	m_notebook->AddPage( m_customBuildPage, wxT("Custom Build"), false );
	m_customMakefileStep = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->AddGrowableRow( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText25 = new wxStaticText( m_customMakefileStep, wxID_ANY, wxT("Dependencies:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer5->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textDeps = new wxTextCtrl( m_customMakefileStep, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textDeps, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText26 = new wxStaticText( m_customMakefileStep, wxID_ANY, wxT("Rule action:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer5->Add( m_staticText26, 0, wxALL, 5 );
	
	m_textPreBuildRule = new wxTextCtrl( m_customMakefileStep, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	fgSizer5->Add( m_textPreBuildRule, 0, wxALL|wxEXPAND, 5 );
	
	bSizer16->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_customMakefileStep, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText24 = new wxStaticText( m_customMakefileStep, wxID_ANY, wxT("Define here a custom makefile rule to be executed in the pre-build steps.\nSee the wiki for more help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	sbSizer2->Add( m_staticText24, 0, wxALL, 5 );
	
	bSizer16->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	m_customMakefileStep->SetSizer( bSizer16 );
	m_customMakefileStep->Layout();
	bSizer16->Fit( m_customMakefileStep );
	m_notebook->AddPage( m_customMakefileStep, wxT("Custom makefile steps"), false );
	
	bSizer94->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer94 );
	this->Layout();
	
	// Connect Events
	m_choiceProjectTypes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_choiceCompilerType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlItermediateDir->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseIM_WD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseIntermediateDir ), NULL, this );
	m_textCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseProgram->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseProgram ), NULL, this );
	m_textCommandArguments->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCommandWD->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCommandWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseCommandWD ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkCompilerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCheckCompilerNeeded ), NULL, this );
	m_textCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCompilerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddCompilerOptions ), NULL, this );
	m_textAdditionalSearchPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddSearchPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddSearchPath ), NULL, this );
	m_textPreprocessor->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddPreprocessor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddPreprocessor ), NULL, this );
	m_textCtrlPreCompiledHeader->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowsePreCmpHeader ), NULL, this );
	m_checkLinkerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCheckLinkerNeeded ), NULL, this );
	m_textLinkerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLinkerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddLinkerOptions ), NULL, this );
	m_textLibraryPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraryPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddLibraryPath ), NULL, this );
	m_textLibraries->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraries->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddLibrary ), NULL, this );
	m_textCtrlDebuggerPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonSelectDebugger->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnSelectDebuggerPath ), NULL, this );
	m_checkBoxDbgRemote->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDebuggingRemoteTarget ), NULL, this );
	m_textCtrl1DbgHost->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgCmds->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPostConnectCmds->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkResourceNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpNeeded ), NULL, this );
	m_textAddResCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpAddOption ), NULL, this );
	m_buttonAddResCmpPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpAddPath ), NULL, this );
	m_checkListPreBuildCommands->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewPreBuildCommand ), NULL, this );
	m_buttonDeletePreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeletePreBuildCommand ), NULL, this );
	m_buttonEditPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPreBuildCommand ), NULL, this );
	m_buttonUpPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnUpPreBuildCommand ), NULL, this );
	m_buttonDownPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDownPreBuildCommand ), NULL, this );
	m_checkListPostBuildCommands->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPostBuildCommand ), NULL, this );
	m_checkListPostBuildCommands->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewPostBuildCommand ), NULL, this );
	m_buttonDeletePostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeletePostBuildCommand ), NULL, this );
	m_buttonEditPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPostBuildCommand ), NULL, this );
	m_buttonUpPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnUpPostBuildCommand ), NULL, this );
	m_buttonDownPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDownPostBuildCommand ), NULL, this );
	m_checkEnableCustomBuild->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCustomBuildEnabled ), NULL, this );
	m_textCtrlCustomBuildWD->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCustomBuildWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseCustomBuildWD ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ProjectConfigurationBasePanel::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ProjectConfigurationBasePanel::OnItemSelected ), NULL, this );
	m_buttonNewCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewTarget ), NULL, this );
	m_buttonEditCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditTarget ), NULL, this );
	m_buttonEditCustomTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectConfigurationBasePanel::OnEditTargetUI ), NULL, this );
	m_buttonDeleteCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeleteTarget ), NULL, this );
	m_buttonDeleteCustomTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectConfigurationBasePanel::OnDeleteTargetUI ), NULL, this );
	m_thirdPartyTool->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnChoiceMakefileTool ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
}

ProjectConfigurationBasePanel::~ProjectConfigurationBasePanel()
{
	// Disconnect Events
	m_choiceProjectTypes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_choiceCompilerType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlItermediateDir->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseIM_WD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseIntermediateDir ), NULL, this );
	m_textCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseProgram->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseProgram ), NULL, this );
	m_textCommandArguments->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCommandWD->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCommandWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseCommandWD ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkCompilerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCheckCompilerNeeded ), NULL, this );
	m_textCompilerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCompilerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddCompilerOptions ), NULL, this );
	m_textAdditionalSearchPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddSearchPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddSearchPath ), NULL, this );
	m_textPreprocessor->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddPreprocessor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddPreprocessor ), NULL, this );
	m_textCtrlPreCompiledHeader->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowsePreCmpHeader ), NULL, this );
	m_checkLinkerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCheckLinkerNeeded ), NULL, this );
	m_textLinkerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLinkerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnButtonAddLinkerOptions ), NULL, this );
	m_textLibraryPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraryPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddLibraryPath ), NULL, this );
	m_textLibraries->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraries->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnAddLibrary ), NULL, this );
	m_textCtrlDebuggerPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonSelectDebugger->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnSelectDebuggerPath ), NULL, this );
	m_checkBoxDbgRemote->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDebuggingRemoteTarget ), NULL, this );
	m_textCtrl1DbgHost->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgCmds->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPostConnectCmds->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkResourceNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpNeeded ), NULL, this );
	m_textAddResCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpAddOption ), NULL, this );
	m_buttonAddResCmpPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnResourceCmpAddPath ), NULL, this );
	m_checkListPreBuildCommands->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewPreBuildCommand ), NULL, this );
	m_buttonDeletePreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeletePreBuildCommand ), NULL, this );
	m_buttonEditPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPreBuildCommand ), NULL, this );
	m_buttonUpPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnUpPreBuildCommand ), NULL, this );
	m_buttonDownPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDownPreBuildCommand ), NULL, this );
	m_checkListPostBuildCommands->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPostBuildCommand ), NULL, this );
	m_checkListPostBuildCommands->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewPostBuildCommand ), NULL, this );
	m_buttonDeletePostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeletePostBuildCommand ), NULL, this );
	m_buttonEditPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditPostBuildCommand ), NULL, this );
	m_buttonUpPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnUpPostBuildCommand ), NULL, this );
	m_buttonDownPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDownPostBuildCommand ), NULL, this );
	m_checkEnableCustomBuild->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCustomBuildEnabled ), NULL, this );
	m_textCtrlCustomBuildWD->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCustomBuildWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnBrowseCustomBuildWD ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( ProjectConfigurationBasePanel::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ProjectConfigurationBasePanel::OnItemSelected ), NULL, this );
	m_buttonNewCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnNewTarget ), NULL, this );
	m_buttonEditCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnEditTarget ), NULL, this );
	m_buttonEditCustomTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectConfigurationBasePanel::OnEditTargetUI ), NULL, this );
	m_buttonDeleteCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnDeleteTarget ), NULL, this );
	m_buttonDeleteCustomTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectConfigurationBasePanel::OnDeleteTargetUI ), NULL, this );
	m_thirdPartyTool->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnChoiceMakefileTool ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectConfigurationBasePanel::OnCmdEvtVModified ), NULL, this );
}

GlobalSettingsBasePanel::GlobalSettingsBasePanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer117;
	bSizer117 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_compilerPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* compilerPageSizer;
	compilerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer10;
	fgSizer10 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer10->AddGrowableCol( 1 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer10->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_textCompilerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonCompilerOptions, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer10->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_textAdditionalSearchPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonAddSearchPath, 0, wxALL, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer10->Add( m_staticText171, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_textPreprocessor, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonAddPreprocessor, 0, wxALL, 5 );
	
	compilerPageSizer->Add( fgSizer10, 0, wxEXPAND, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	m_notebook->AddPage( m_compilerPage, wxT("Compiler"), true );
	m_linkerPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer11->AddGrowableCol( 1 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer11->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_textLinkerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLinkerOptions, 0, wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Library Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer11->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_textLibraryPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLibraryPath, 0, wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer11->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_textLibraries, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLibraries, 0, wxALL, 5 );
	
	linkerPageSizer->Add( fgSizer11, 0, wxEXPAND, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	m_notebook->AddPage( m_linkerPage, wxT("Linker"), false );
	m_resourceCmpPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText221 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer4->Add( m_staticText221, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpOptions = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpOptions, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpOptions = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpOptions, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText23 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer4->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpPath = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpPath, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpPath = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpPath, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer151->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_resourceCmpPage->SetSizer( bSizer151 );
	m_resourceCmpPage->Layout();
	bSizer151->Fit( m_resourceCmpPage );
	m_notebook->AddPage( m_resourceCmpPage, wxT("Resources"), false );
	
	bSizer117->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer117 );
	this->Layout();
	
	// Connect Events
	m_textCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCompilerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddCompilerOptions ), NULL, this );
	m_textAdditionalSearchPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddSearchPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddSearchPath ), NULL, this );
	m_textPreprocessor->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddPreprocessor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddPreprocessor ), NULL, this );
	m_textLinkerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLinkerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddLinkerOptions ), NULL, this );
	m_textLibraryPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraryPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddLibraryPath ), NULL, this );
	m_textLibraries->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraries->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddLibrary ), NULL, this );
	m_textAddResCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnResourceCmpAddOption ), NULL, this );
	m_textAddResCmpPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnResourceCmpAddPath ), NULL, this );
}

GlobalSettingsBasePanel::~GlobalSettingsBasePanel()
{
	// Disconnect Events
	m_textCompilerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCompilerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddCompilerOptions ), NULL, this );
	m_textAdditionalSearchPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddSearchPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddSearchPath ), NULL, this );
	m_textPreprocessor->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddPreprocessor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddPreprocessor ), NULL, this );
	m_textLinkerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLinkerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddLinkerOptions ), NULL, this );
	m_textLibraryPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraryPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddLibraryPath ), NULL, this );
	m_textLibraries->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonLibraries->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnAddLibrary ), NULL, this );
	m_textAddResCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnResourceCmpAddOption ), NULL, this );
	m_textAddResCmpPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonAddResCmpPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnResourceCmpAddPath ), NULL, this );
}
