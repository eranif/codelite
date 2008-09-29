///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	m_notebook3 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_generalPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 4, 2, 0, 0 );
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
	
	m_staticText191 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	fgSizer3->Add( m_staticText191, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCompilerTypeChoices;
	m_choiceCompilerType = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCompilerTypeChoices, 0 );
	m_choiceCompilerType->SetSelection( 0 );
	fgSizer3->Add( m_choiceCompilerType, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText231 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Debugger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	fgSizer3->Add( m_staticText231, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDebuggerChoices;
	m_choiceDebugger = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggerChoices, 0 );
	m_choiceDebugger->SetSelection( 0 );
	fgSizer3->Add( m_choiceDebugger, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Output File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer3->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textOutputFilePicker = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textOutputFilePicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Intermediate Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_intermediateDirPicker = new DirPicker(m_generalPage);
	fgSizer3->Add( m_intermediateDirPicker, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Action:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	bSizer21->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline5 = new wxStaticLine( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer21->Add( m_staticline5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer19->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText18 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer6->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommand = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommand, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText19 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Command Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer6->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommandArguments = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommandArguments, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer6->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_workingDirPicker = new DirPicker(m_generalPage);
	fgSizer6->Add( m_workingDirPicker, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( fgSizer6, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxPauseWhenExecEnds = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Pause when execution ends"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer17->Add( m_checkBoxPauseWhenExecEnds, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( bSizer17, 1, wxEXPAND, 5 );
	
	m_generalPage->SetSizer( bSizer19 );
	m_generalPage->Layout();
	bSizer19->Fit( m_generalPage );
	m_notebook3->AddPage( m_generalPage, wxT("General"), true );
	m_compilerPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* compilerPageSizer;
	compilerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkCompilerNeeded = new wxCheckBox( m_compilerPage, wxID_ANY, wxT("Compiler is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	
	compilerPageSizer->Add( m_checkCompilerNeeded, 0, wxALL, 5 );
	
	m_staticline7 = new wxStaticLine( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	compilerPageSizer->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 0, 0 );
	gbSizer1->AddGrowableCol( 1 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	gbSizer1->Add( m_staticText6, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textCompilerOptions, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	gbSizer1->Add( m_staticText4, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textAdditionalSearchPath, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer1->Add( m_buttonAddSearchPath, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	gbSizer1->Add( m_staticText171, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textPreprocessor, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer1->Add( m_buttonAddPreprocessor, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer1->Add( m_buttonCompilerOptions, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	compilerPageSizer->Add( gbSizer1, 1, wxEXPAND, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	m_notebook3->AddPage( m_compilerPage, wxT("Compiler"), false );
	m_linkerPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkLinkerNeeded = new wxCheckBox( m_linkerPage, wxID_ANY, wxT("Linker is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	
	linkerPageSizer->Add( m_checkLinkerNeeded, 0, wxALL, 5 );
	
	m_staticline8 = new wxStaticLine( m_linkerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	linkerPageSizer->Add( m_staticline8, 0, wxEXPAND | wxALL, 5 );
	
	wxGridBagSizer* gbSizer2;
	gbSizer2 = new wxGridBagSizer( 0, 0 );
	gbSizer2->AddGrowableCol( 1 );
	gbSizer2->SetFlexibleDirection( wxBOTH );
	gbSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	gbSizer2->Add( m_staticText10, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLinkerOptions, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Library Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	gbSizer2->Add( m_staticText7, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLibraryPath, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer2->Add( m_buttonLibraries, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	gbSizer2->Add( m_staticText8, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLibraries, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer2->Add( m_buttonLibraryPath, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	gbSizer2->Add( m_buttonLinkerOptions, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	linkerPageSizer->Add( gbSizer2, 1, wxEXPAND, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	m_notebook3->AddPage( m_linkerPage, wxT("Linker"), false );
	m_panelDebugger = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer192;
	bSizer192 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxDbgRemote = new wxCheckBox( m_panelDebugger, wxID_ANY, wxT("Debugging remote target"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer192->Add( m_checkBoxDbgRemote, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer61;
	fgSizer61 = new wxFlexGridSizer( 2, 4, 0, 0 );
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
	
	m_staticText301 = new wxStaticText( m_panelDebugger, wxID_ANY, wxT("Enter here any commands that should be passed to the debugger on startup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	bSizer192->Add( m_staticText301, 0, wxALL, 5 );
	
	m_textCtrlDbgCmds = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxSIMPLE_BORDER );
	bSizer192->Add( m_textCtrlDbgCmds, 1, wxALL|wxEXPAND, 5 );
	
	m_panelDebugger->SetSizer( bSizer192 );
	m_panelDebugger->Layout();
	bSizer192->Fit( m_panelDebugger );
	m_notebook3->AddPage( m_panelDebugger, wxT("Debugger"), false );
	m_resourceCmpPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	m_checkResourceNeeded = new wxCheckBox( m_resourceCmpPage, wxID_ANY, wxT("Resource Compiler is not needed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkResourceNeeded->SetValue(true);
	
	bSizer151->Add( m_checkResourceNeeded, 0, wxALL, 5 );
	
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
	fgSizer4->Add( m_textAddResCmpOptions, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddResCmpOptions = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer4->Add( m_buttonAddResCmpOptions, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText23 = new wxStaticText( m_resourceCmpPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer4->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpPath = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpPath, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddResCmpPath = new wxButton( m_resourceCmpPage, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	fgSizer4->Add( m_buttonAddResCmpPath, 0, wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	bSizer151->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_resourceCmpPage->SetSizer( bSizer151 );
	m_resourceCmpPage->Layout();
	bSizer151->Fit( m_resourceCmpPage );
	m_notebook3->AddPage( m_resourceCmpPage, wxT("Resources"), false );
	m_preBuildPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	m_notebook3->AddPage( m_preBuildPage, wxT("PreBuild"), false );
	m_postBuildPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	m_notebook3->AddPage( m_postBuildPage, wxT("PostBuild"), false );
	m_customBuildPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_checkEnableCustomBuild = new wxCheckBox( m_customBuildPage, wxID_ANY, wxT("Enable custom build"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer15->Add( m_checkEnableCustomBuild, 0, wxALL, 5 );
	
	m_staticline82 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer15->Add( m_staticline82, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer31->AddGrowableCol( 1 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText27 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Working directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer31->Add( m_staticText27, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_customBuildDirPicker = new DirPicker(m_customBuildPage);
	fgSizer31->Add( m_customBuildDirPicker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText181 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Build Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText181->Wrap( -1 );
	fgSizer31->Add( m_staticText181, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textBuildCommand = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textBuildCommand, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText192 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Clean Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText192->Wrap( -1 );
	fgSizer31->Add( m_staticText192, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCleanCommand = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCleanCommand, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText291 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Single File Build Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText291->Wrap( -1 );
	fgSizer31->Add( m_staticText291, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl1SingleFileCommand = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textCtrl1SingleFileCommand, 0, wxALL|wxEXPAND, 5 );
	
	bSizer15->Add( fgSizer31, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonCustomTargets = new wxButton( m_customBuildPage, wxID_ANY, wxT("More Custom Targets..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCustomTargets->SetToolTip( wxT("Click on this button to add more targets other than the 'built-in' targets Build and Clean (e.g. make distclean)\nthese commands will be available from the project context menu -> 'Custom Build Targets' sub menu") );
	
	bSizer20->Add( m_buttonCustomTargets, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( bSizer20, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText29 = new wxStaticText( m_customBuildPage, wxID_ANY, wxT("Makefile Generators:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer191->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline11 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer191->Add( m_staticline11, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( bSizer191, 0, wxEXPAND, 5 );
	
	wxString m_thirdPartyToolChoices[] = { wxT("None"), wxT("Premake"), wxT("QMake"), wxT("CMake"), wxT("configure") };
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
	m_notebook3->AddPage( m_customBuildPage, wxT("Custom Build"), false );
	m_customMakefileStep = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
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
	
	m_textPreBuildRule = new wxTextCtrl( m_customMakefileStep, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2|wxSIMPLE_BORDER );
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
	m_notebook3->AddPage( m_customMakefileStep, wxT("Custom makefile steps"), false );
	
	mainSizer->Add( m_notebook3, 1, wxEXPAND | wxALL, 5 );
	
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
	m_choiceProjectTypes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_choiceCompilerType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCommandArguments->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkCompilerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAdditionalSearchPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textPreprocessor->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkLinkerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLinkerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLibraryPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLibraries->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkBoxDbgRemote->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnDebuggingRemoteTarget ), NULL, this );
	m_textCtrl1DbgHost->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgCmds->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkResourceNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDeletePreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonEditPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonUpPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDownPreBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPostBuildCommands->Connect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPostBuildCommands->Connect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDeletePostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonEditPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonUpPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDownPostBuildCmd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkEnableCustomBuild->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textBuildCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCleanCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrl1SingleFileCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonCustomTargets->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCustomTargets ), NULL, this );
	m_thirdPartyTool->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnChoiceMakefileTool ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
}

ProjectSettingsBaseDlg::~ProjectSettingsBaseDlg()
{
	// Disconnect Events
	m_choiceProjectTypes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_choiceCompilerType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCommandArguments->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkCompilerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCompilerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAdditionalSearchPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textPreprocessor->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkLinkerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLinkerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLibraryPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textLibraries->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkBoxDbgRemote->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnDebuggingRemoteTarget ), NULL, this );
	m_textCtrl1DbgHost->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgCmds->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkResourceNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPreBuildCommands->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDeletePreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonEditPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonUpPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDownPreBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPostBuildCommands->Disconnect( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkListPostBuildCommands->Disconnect( wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonNewPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDeletePostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonEditPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonUpPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonDownPostBuildCmd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_checkEnableCustomBuild->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textBuildCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCleanCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textCtrl1SingleFileCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonCustomTargets->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCustomTargets ), NULL, this );
	m_thirdPartyTool->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnChoiceMakefileTool ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnCmdEvtVModified ), NULL, this );
	m_buttonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
}
