///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
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
	
	m_panelSettings = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_sizerSettings = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_choiceConfigChoices;
	m_choiceConfig = new wxChoice( m_panelSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceConfigChoices, 0 );
	m_choiceConfig->SetSelection( 0 );
	bSizer44->Add( m_choiceConfig, 1, wxALL|wxEXPAND, 5 );
	
	m_sizerSettings->Add( bSizer44, 0, wxEXPAND, 5 );
	
	m_treebook = new wxTreebook( m_panelSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_sizerSettings->Add( m_treebook, 1, wxALL|wxEXPAND, 5 );
	
	m_panelSettings->SetSizer( m_sizerSettings );
	m_panelSettings->Layout();
	m_sizerSettings->Fit( m_panelSettings );
	mainSizer->Add( m_panelSettings, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonHelp = new wxButton( this, wxID_ANY, _("Help..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonHelp, 0, wxALL, 5 );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonApply, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_choiceConfig->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnConfigurationChanged ), NULL, this );
	m_buttonHelp->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonApply ), NULL, this );
	m_buttonApply->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectSettingsBaseDlg::OnButtonApplyUI ), NULL, this );
}

ProjectSettingsBaseDlg::~ProjectSettingsBaseDlg()
{
	// Disconnect Events
	m_choiceConfig->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnConfigurationChanged ), NULL, this );
	m_buttonHelp->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonHelp ), NULL, this );
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonOK ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ProjectSettingsBaseDlg::OnButtonApply ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( ProjectSettingsBaseDlg::OnButtonApplyUI ), NULL, this );
}

PSGeneralPageBase::PSGeneralPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_generalPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_gbSizer1 = new wxGridBagSizer( 0, 0 );
	m_gbSizer1->SetFlexibleDirection( wxBOTH );
	m_gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText22 = new wxStaticText( m_generalPage, wxID_ANY, _("Project Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	m_gbSizer1->Add( m_staticText22, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceProjectTypesChoices;
	m_choiceProjectTypes = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjectTypesChoices, 0 );
	m_choiceProjectTypes->SetSelection( 0 );
	m_gbSizer1->Add( m_choiceProjectTypes, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticText191 = new wxStaticText( m_generalPage, wxID_ANY, _("Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText191->Wrap( -1 );
	m_gbSizer1->Add( m_staticText191, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCompilerTypeChoices;
	m_choiceCompilerType = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCompilerTypeChoices, 0 );
	m_choiceCompilerType->SetSelection( 0 );
	m_gbSizer1->Add( m_choiceCompilerType, wxGBPosition( 1, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticText231 = new wxStaticText( m_generalPage, wxID_ANY, _("Debugger:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	m_gbSizer1->Add( m_staticText231, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDebuggerChoices;
	m_choiceDebugger = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDebuggerChoices, 0 );
	m_choiceDebugger->SetSelection( 0 );
	m_gbSizer1->Add( m_choiceDebugger, wxGBPosition( 2, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( m_generalPage, wxID_ANY, _("Output File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	m_gbSizer1->Add( m_staticText15, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textOutputFilePicker = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textOutputFilePicker, wxGBPosition( 3, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_generalPage, wxID_ANY, _("Intermediate Folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	m_gbSizer1->Add( m_staticText16, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlItermediateDir = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textCtrlItermediateDir, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseIM_WD = new wxButton( m_generalPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer1->Add( m_buttonBrowseIM_WD, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText17 = new wxStaticText( m_generalPage, wxID_ANY, _("Program to Debug / Run:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	m_gbSizer1->Add( m_staticText17, wxGBPosition( 5, 0 ), wxGBSpan( 1, 3 ), wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText18 = new wxStaticText( m_generalPage, wxID_ANY, _("Program:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	m_gbSizer1->Add( m_staticText18, wxGBPosition( 6, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommand = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textCommand, wxGBPosition( 6, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseProgram = new wxButton( m_generalPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer1->Add( m_buttonBrowseProgram, wxGBPosition( 6, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText19 = new wxStaticText( m_generalPage, wxID_ANY, _("Program Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_gbSizer1->Add( m_staticText19, wxGBPosition( 8, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommandArguments = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textCommandArguments, wxGBPosition( 8, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxUseDebugArgs = new wxCheckBox( m_generalPage, wxID_ANY, _("Use separate debug arguments"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseDebugArgs->SetToolTip( _("When enabled, codelite will use the arguments specified in the 'Debug Arguments' when running the program under debugger. \nOtherwise, codelite will use the arguments speicified in the 'Program Arguments' field") );
	
	m_gbSizer1->Add( m_checkBoxUseDebugArgs, wxGBPosition( 9, 0 ), wxGBSpan( 1, 3 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText44 = new wxStaticText( m_generalPage, wxID_ANY, _("Debug Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	m_gbSizer1->Add( m_staticText44, wxGBPosition( 10, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlDebugArgs = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textCtrlDebugArgs, wxGBPosition( 10, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( m_generalPage, wxID_ANY, _("Working Folder:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	m_gbSizer1->Add( m_staticText20, wxGBPosition( 7, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCommandWD = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_gbSizer1->Add( m_textCtrlCommandWD, wxGBPosition( 7, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonBrowseCommandWD = new wxButton( m_generalPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer1->Add( m_buttonBrowseCommandWD, wxGBPosition( 7, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer19->Add( m_gbSizer1, 0, wxEXPAND, 5 );
	
	m_staticline11 = new wxStaticLine( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer19->Add( m_staticline11, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxPauseWhenExecEnds = new wxCheckBox( m_generalPage, wxID_ANY, _("Pause when execution ends"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( m_checkBoxPauseWhenExecEnds, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( bSizer17, 0, wxEXPAND, 5 );
	
	m_generalPage->SetSizer( bSizer19 );
	m_generalPage->Layout();
	bSizer19->Fit( m_generalPage );
	bSizer35->Add( m_generalPage, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer35 );
	this->Layout();
	bSizer35->Fit( this );
	
	// Connect Events
	m_staticText22->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_choiceProjectTypes->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_choiceProjectTypes->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_choiceCompilerType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlItermediateDir->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseIM_WD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseIntermediateDir ), NULL, this );
	m_textCommand->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseProgram->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseProgram ), NULL, this );
	m_textCommandArguments->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxUseDebugArgs->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_staticText44->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnUseDebugArgsUI ), NULL, this );
	m_textCtrlDebugArgs->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDebugArgs->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnUseDebugArgsUI ), NULL, this );
	m_textCtrlCommandWD->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCommandWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseCommandWD ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
}

PSGeneralPageBase::~PSGeneralPageBase()
{
	// Disconnect Events
	m_staticText22->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_choiceProjectTypes->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_choiceProjectTypes->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_choiceCompilerType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_choiceDebugger->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textOutputFilePicker->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlItermediateDir->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseIM_WD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseIntermediateDir ), NULL, this );
	m_textCommand->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseProgram->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseProgram ), NULL, this );
	m_textCommandArguments->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxUseDebugArgs->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_staticText44->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnUseDebugArgsUI ), NULL, this );
	m_textCtrlDebugArgs->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDebugArgs->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSGeneralPageBase::OnUseDebugArgsUI ), NULL, this );
	m_textCtrlCommandWD->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonBrowseCommandWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnBrowseCommandWD ), NULL, this );
	m_checkBoxPauseWhenExecEnds->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSGeneralPageBase::OnCmdEvtVModified ), NULL, this );
}

PSCompilerPageBase::PSCompilerPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_compilerPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* compilerPageSizer;
	compilerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkCompilerNeeded = new wxCheckBox( m_compilerPage, wxID_ANY, _("Compiler is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	compilerPageSizer->Add( m_checkCompilerNeeded, 0, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_compilerPage, wxID_ANY, _("Compiler Options:") ), wxVERTICAL );
	
	m_gbSizer2 = new wxGridBagSizer( 0, 0 );
	m_gbSizer2->SetFlexibleDirection( wxBOTH );
	m_gbSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText331 = new wxStaticText( m_compilerPage, wxID_ANY, _("Use with Global Settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText331->Wrap( -1 );
	m_gbSizer2->Add( m_staticText331, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCmpUseWithGlobalSettingsChoices;
	m_choiceCmpUseWithGlobalSettings = new wxChoice( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCmpUseWithGlobalSettingsChoices, 0 );
	m_choiceCmpUseWithGlobalSettings->SetSelection( 0 );
	m_gbSizer2->Add( m_choiceCmpUseWithGlobalSettings, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline7 = new wxStaticLine( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_gbSizer2->Add( m_staticline7, wxGBPosition( 1, 0 ), wxGBSpan( 1, 3 ), wxEXPAND|wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, _("C++ Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	m_staticText6->SetToolTip( _("Additional compiler options to pass to the compiler provided as a semi-colon delimited list") );
	
	m_gbSizer2->Add( m_staticText6, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textCompilerOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCompilerOptions->SetToolTip( _("Additional compiler options to pass to the compiler provided as a semi-colon delimited list") );
	
	m_gbSizer2->Add( m_textCompilerOptions, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_gbSizer2->Add( m_buttonCompilerOptions, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText45 = new wxStaticText( m_compilerPage, wxID_ANY, _("C Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	m_gbSizer2->Add( m_staticText45, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textCtrlCCompilerOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlCCompilerOptions->SetToolTip( _("Additional C compiler options to pass to the compiler provided as a semi-colon delimited list (used for C files only)") );
	
	m_gbSizer2->Add( m_textCtrlCCompilerOptions, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button35 = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_gbSizer2->Add( m_button35, wxGBPosition( 3, 2 ), wxGBSpan( 1, 1 ), wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, _("Include Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetToolTip( _("Include path to pass to the compiler (provided as semi-colon delimited list)") );
	
	m_gbSizer2->Add( m_staticText4, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textAdditionalSearchPath->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textAdditionalSearchPath->SetToolTip( _("Include path to pass to the compiler (provided as semi-colon delimited list)") );
	
	m_gbSizer2->Add( m_textAdditionalSearchPath, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_gbSizer2->Add( m_buttonAddSearchPath, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, _("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	m_staticText171->SetToolTip( _("macros (\"defines\") to pass to the compiler (provided as semi-colon delimited list)") );
	
	m_gbSizer2->Add( m_staticText171, wxGBPosition( 5, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textPreprocessor->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textPreprocessor->SetToolTip( _("macros (\"defines\") to pass to the compiler (provided as semi-colon delimited list)") );
	
	m_gbSizer2->Add( m_textPreprocessor, wxGBPosition( 5, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_gbSizer2->Add( m_buttonAddPreprocessor, wxGBPosition( 5, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer3->Add( m_gbSizer2, 0, wxEXPAND, 5 );
	
	compilerPageSizer->Add( sbSizer3, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_compilerPage, wxID_ANY, _("Pre Compiled Headers:") ), wxVERTICAL );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlPreCompiledHeader = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textCtrlPreCompiledHeader->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlPreCompiledHeader->SetToolTip( _("Specify here the pre-compiled header to use") );
	
	bSizer33->Add( m_textCtrlPreCompiledHeader, 1, wxALL, 5 );
	
	m_buttonBrowsePreCompiledHeader = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer33->Add( m_buttonBrowsePreCompiledHeader, 0, wxALL, 5 );
	
	sbSizer2->Add( bSizer33, 0, wxEXPAND, 5 );
	
	m_checkBoxPCHInCommandLine = new wxCheckBox( m_compilerPage, wxID_ANY, _("Explicitly include the PCH file in the command line using compiler switch"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxPCHInCommandLine, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxSeparatePCHFlags = new wxCheckBox( m_compilerPage, wxID_ANY, _("Use separate compilation flags for the PCH file:"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxSeparatePCHFlags, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlPCHCompilationFlags = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textCtrlPCHCompilationFlags->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlPCHCompilationFlags->SetToolTip( _("Specify here different compiler flags for building the PCH file") );
	
	sbSizer2->Add( m_textCtrlPCHCompilationFlags, 0, wxALL|wxEXPAND, 5 );
	
	compilerPageSizer->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	bSizer36->Add( m_compilerPage, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer36 );
	this->Layout();
	bSizer36->Fit( this );
	
	// Connect Events
	m_checkCompilerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCheckCompilerNeeded ), NULL, this );
	m_checkCompilerNeeded->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText331->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_choiceCmpUseWithGlobalSettings->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticline7->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText6->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCompilerOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonCompilerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddCompilerOptions ), NULL, this );
	m_buttonCompilerOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText45->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlCCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCCompilerOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_button35->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddCCompilerOptions ), NULL, this );
	m_button35->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textAdditionalSearchPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAdditionalSearchPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonAddSearchPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnAddSearchPath ), NULL, this );
	m_buttonAddSearchPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText171->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textPreprocessor->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textPreprocessor->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonAddPreprocessor->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddPreprocessor ), NULL, this );
	m_buttonAddPreprocessor->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlPreCompiledHeader->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlPreCompiledHeader->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnBrowsePreCmpHeader ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_checkBoxPCHInCommandLine->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxPCHInCommandLine->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_checkBoxSeparatePCHFlags->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxSeparatePCHFlags->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlPCHCompilationFlags->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlPCHCompilationFlags->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnEnablePCHFLagsUI ), NULL, this );
}

PSCompilerPageBase::~PSCompilerPageBase()
{
	// Disconnect Events
	m_checkCompilerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCheckCompilerNeeded ), NULL, this );
	m_checkCompilerNeeded->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText331->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_choiceCmpUseWithGlobalSettings->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticline7->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText6->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCompilerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCompilerOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonCompilerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddCompilerOptions ), NULL, this );
	m_buttonCompilerOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText45->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlCCompilerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCCompilerOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_button35->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddCCompilerOptions ), NULL, this );
	m_button35->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textAdditionalSearchPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAdditionalSearchPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonAddSearchPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnAddSearchPath ), NULL, this );
	m_buttonAddSearchPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_staticText171->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textPreprocessor->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textPreprocessor->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonAddPreprocessor->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnButtonAddPreprocessor ), NULL, this );
	m_buttonAddPreprocessor->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlPreCompiledHeader->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlPreCompiledHeader->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnBrowsePreCmpHeader ), NULL, this );
	m_buttonBrowsePreCompiledHeader->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_checkBoxPCHInCommandLine->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxPCHInCommandLine->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_checkBoxSeparatePCHFlags->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxSeparatePCHFlags->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnCompiledNotNeededUI ), NULL, this );
	m_textCtrlPCHCompilationFlags->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompilerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlPCHCompilationFlags->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCompilerPageBase::OnEnablePCHFLagsUI ), NULL, this );
}

PSLinkPageBase::PSLinkPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_linkerPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkLinkerNeeded = new wxCheckBox( m_linkerPage, wxID_ANY, _("Linker is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	linkerPageSizer->Add( m_checkLinkerNeeded, 0, wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_linkerPage, wxID_ANY, _("Linker Options:") ), wxVERTICAL );
	
	m_gbSizer3 = new wxGridBagSizer( 0, 0 );
	m_gbSizer3->SetFlexibleDirection( wxBOTH );
	m_gbSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3311 = new wxStaticText( m_linkerPage, wxID_ANY, _("Use with global settings :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3311->Wrap( -1 );
	m_gbSizer3->Add( m_staticText3311, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceLnkUseWithGlobalSettingsChoices;
	m_choiceLnkUseWithGlobalSettings = new wxChoice( m_linkerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceLnkUseWithGlobalSettingsChoices, 0 );
	m_choiceLnkUseWithGlobalSettings->SetSelection( 0 );
	m_gbSizer3->Add( m_choiceLnkUseWithGlobalSettings, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	m_gbSizer3->Add( 0, 0, wxGBPosition( 1, 3 ), wxGBSpan( 1, 1 ), wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, _("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	m_gbSizer3->Add( m_staticText10, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textLinkerOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	m_gbSizer3->Add( m_textLinkerOptions, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer3->Add( m_buttonLinkerOptions, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, _("Library Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_gbSizer3->Add( m_staticText7, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textLibraryPath->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	m_gbSizer3->Add( m_textLibraryPath, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer3->Add( m_buttonLibraryPath, wxGBPosition( 3, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, _("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	m_gbSizer3->Add( m_staticText8, wxGBPosition( 4, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textLibraries->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textLibraries->SetToolTip( _("Enter any extra library names, separated by';' e.g. Foo  or  Foo;Bar") );
	
	m_gbSizer3->Add( m_textLibraries, wxGBPosition( 4, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer3->Add( m_buttonLibraries, wxGBPosition( 4, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer4->Add( m_gbSizer3, 1, wxEXPAND, 5 );
	
	linkerPageSizer->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	bSizer37->Add( m_linkerPage, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer37 );
	this->Layout();
	bSizer37->Fit( this );
	
	// Connect Events
	m_checkLinkerNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnCheckLinkerNeeded ), NULL, this );
	m_checkLinkerNeeded->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText3311->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_choiceLnkUseWithGlobalSettings->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText10->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLinkerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLinkerOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLinkerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnButtonAddLinkerOptions ), NULL, this );
	m_buttonLinkerOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText7->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLibraryPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLibraryPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLibraryPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnAddLibraryPath ), NULL, this );
	m_buttonLibraryPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText8->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLibraries->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLibraries->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLibraries->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnAddLibrary ), NULL, this );
	m_buttonLibraries->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
}

PSLinkPageBase::~PSLinkPageBase()
{
	// Disconnect Events
	m_checkLinkerNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnCheckLinkerNeeded ), NULL, this );
	m_checkLinkerNeeded->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText3311->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_choiceLnkUseWithGlobalSettings->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText10->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLinkerOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLinkerOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLinkerOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnButtonAddLinkerOptions ), NULL, this );
	m_buttonLinkerOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText7->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLibraryPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLibraryPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLibraryPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnAddLibraryPath ), NULL, this );
	m_buttonLibraryPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_staticText8->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_textLibraries->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSLinkPageBase::OnCmdEvtVModified ), NULL, this );
	m_textLibraries->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
	m_buttonLibraries->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSLinkPageBase::OnAddLibrary ), NULL, this );
	m_buttonLibraries->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSLinkPageBase::OnLinkerNotNeededUI ), NULL, this );
}

PSDebuggerPageBase::PSDebuggerPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_panelDebugger = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer192;
	bSizer192 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText321 = new wxStaticText( m_panelDebugger, wxID_ANY, _("Select debugger path. Leave empty to use the default:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText321->Wrap( -1 );
	bSizer192->Add( m_staticText321, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer62;
	fgSizer62 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer62->AddGrowableCol( 0 );
	fgSizer62->SetFlexibleDirection( wxBOTH );
	fgSizer62->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_textCtrlDebuggerPath = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textCtrlDebuggerPath->SetToolTip( _("You may override the global debugger executable path, by selecting another one here.\nLeave this field empty if you want to use the one set in the global debugger settings") );
	
	fgSizer62->Add( m_textCtrlDebuggerPath, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_buttonSelectDebugger = new wxButton( m_panelDebugger, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer62->Add( m_buttonSelectDebugger, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer192->Add( fgSizer62, 0, wxEXPAND, 5 );
	
	m_staticline121 = new wxStaticLine( m_panelDebugger, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer192->Add( m_staticline121, 0, wxEXPAND | wxALL, 5 );
	
	m_checkBoxDbgRemote = new wxCheckBox( m_panelDebugger, wxID_ANY, _("Debugging remote target"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer192->Add( m_checkBoxDbgRemote, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer61;
	fgSizer61 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer61->AddGrowableCol( 1 );
	fgSizer61->SetFlexibleDirection( wxBOTH );
	fgSizer61->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText31 = new wxStaticText( m_panelDebugger, wxID_ANY, _("Host / tty:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer61->Add( m_staticText31, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl1DbgHost = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	fgSizer61->Add( m_textCtrl1DbgHost, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText32 = new wxStaticText( m_panelDebugger, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer61->Add( m_staticText32, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlDbgPort = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	fgSizer61->Add( m_textCtrlDbgPort, 0, wxALL, 5 );
	
	bSizer192->Add( fgSizer61, 0, wxRIGHT|wxLEFT|wxEXPAND, 15 );
	
	m_staticline131 = new wxStaticLine( m_panelDebugger, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer192->Add( m_staticline131, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText301 = new wxStaticText( m_panelDebugger, wxID_ANY, _("Enter here any commands that should be passed to the debugger on startup:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText301->Wrap( -1 );
	bSizer192->Add( m_staticText301, 0, wxALL, 5 );
	
	m_textCtrlDbgCmds = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlDbgCmds->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer192->Add( m_textCtrlDbgCmds, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText311 = new wxStaticText( m_panelDebugger, wxID_ANY, _("Enter here any commands that should be passed to the debugger after attaching the remote target:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText311->Wrap( -1 );
	bSizer192->Add( m_staticText311, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlDbgPostConnectCmds = new wxTextCtrl( m_panelDebugger, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlDbgPostConnectCmds->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer192->Add( m_textCtrlDbgPostConnectCmds, 1, wxALL|wxEXPAND, 5 );
	
	m_panelDebugger->SetSizer( bSizer192 );
	m_panelDebugger->Layout();
	bSizer192->Fit( m_panelDebugger );
	bSizer38->Add( m_panelDebugger, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer38 );
	this->Layout();
	bSizer38->Fit( this );
	
	// Connect Events
	m_textCtrlDebuggerPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonSelectDebugger->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSDebuggerPageBase::OnSelectDebuggerPath ), NULL, this );
	m_checkBoxDbgRemote->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_staticText31->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrl1DbgHost->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrl1DbgHost->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_staticText32->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrlDbgPort->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrlDbgCmds->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPostConnectCmds->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
}

PSDebuggerPageBase::~PSDebuggerPageBase()
{
	// Disconnect Events
	m_textCtrlDebuggerPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_buttonSelectDebugger->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSDebuggerPageBase::OnSelectDebuggerPath ), NULL, this );
	m_checkBoxDbgRemote->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_staticText31->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrl1DbgHost->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrl1DbgHost->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_staticText32->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrlDbgPort->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPort->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSDebuggerPageBase::OnRemoteDebugUI ), NULL, this );
	m_textCtrlDbgCmds->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlDbgPostConnectCmds->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSDebuggerPageBase::OnCmdEvtVModified ), NULL, this );
}

PSResourcesPageBase::PSResourcesPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );
	
	m_resourceCmpPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	m_checkResourceNeeded = new wxCheckBox( m_resourceCmpPage, wxID_ANY, _("Resource Compiler is not needed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkResourceNeeded->SetValue(true); 
	bSizer151->Add( m_checkResourceNeeded, 0, wxALL, 5 );
	
	m_gbSizer4 = new wxGridBagSizer( 0, 0 );
	m_gbSizer4->SetFlexibleDirection( wxBOTH );
	m_gbSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText33111 = new wxStaticText( m_resourceCmpPage, wxID_ANY, _("Use with global settings :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33111->Wrap( -1 );
	m_gbSizer4->Add( m_staticText33111, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceResUseWithGlobalSettingsChoices;
	m_choiceResUseWithGlobalSettings = new wxChoice( m_resourceCmpPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceResUseWithGlobalSettingsChoices, 0 );
	m_choiceResUseWithGlobalSettings->SetSelection( 0 );
	m_gbSizer4->Add( m_choiceResUseWithGlobalSettings, wxGBPosition( 0, 1 ), wxGBSpan( 1, 2 ), wxALL|wxEXPAND, 5 );
	
	m_staticline9 = new wxStaticLine( m_resourceCmpPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_gbSizer4->Add( m_staticline9, wxGBPosition( 1, 0 ), wxGBSpan( 1, 3 ), wxEXPAND | wxALL, 5 );
	
	m_staticText221 = new wxStaticText( m_resourceCmpPage, wxID_ANY, _("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	m_gbSizer4->Add( m_staticText221, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpOptions = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textAddResCmpOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	m_gbSizer4->Add( m_textAddResCmpOptions, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_buttonAddResCmpOptions = new wxButton( m_resourceCmpPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer4->Add( m_buttonAddResCmpOptions, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText23 = new wxStaticText( m_resourceCmpPage, wxID_ANY, _("Include Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	m_gbSizer4->Add( m_staticText23, wxGBPosition( 3, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpPath = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textAddResCmpPath->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	m_gbSizer4->Add( m_textAddResCmpPath, wxGBPosition( 3, 1 ), wxGBSpan( 1, 1 ), wxALL|wxEXPAND, 5 );
	
	m_buttonAddResCmpPath = new wxButton( m_resourceCmpPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_gbSizer4->Add( m_buttonAddResCmpPath, wxGBPosition( 3, 2 ), wxGBSpan( 1, 1 ), wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer151->Add( m_gbSizer4, 1, wxEXPAND, 5 );
	
	m_resourceCmpPage->SetSizer( bSizer151 );
	m_resourceCmpPage->Layout();
	bSizer151->Fit( m_resourceCmpPage );
	bSizer39->Add( m_resourceCmpPage, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer39 );
	this->Layout();
	bSizer39->Fit( this );
	
	// Connect Events
	m_checkResourceNeeded->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpNeeded ), NULL, this );
	m_checkResourceNeeded->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText33111->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_choiceResUseWithGlobalSettings->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_staticText221->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_textAddResCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSResourcesPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_buttonAddResCmpOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpAddOption ), NULL, this );
	m_buttonAddResCmpOptions->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_staticText23->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_textAddResCmpPath->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSResourcesPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_buttonAddResCmpPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpAddPath ), NULL, this );
	m_buttonAddResCmpPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
}

PSResourcesPageBase::~PSResourcesPageBase()
{
	// Disconnect Events
	m_checkResourceNeeded->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpNeeded ), NULL, this );
	m_checkResourceNeeded->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnProjectCustumBuildUI ), NULL, this );
	m_staticText33111->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_choiceResUseWithGlobalSettings->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_staticText221->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_textAddResCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSResourcesPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_buttonAddResCmpOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpAddOption ), NULL, this );
	m_buttonAddResCmpOptions->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_staticText23->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_textAddResCmpPath->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSResourcesPageBase::OnCmdEvtVModified ), NULL, this );
	m_textAddResCmpPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
	m_buttonAddResCmpPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSResourcesPageBase::OnResourceCmpAddPath ), NULL, this );
	m_buttonAddResCmpPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSResourcesPageBase::OnrResourceCompilerNotNeededUI ), NULL, this );
}

PSEnvironmentBasePage::PSEnvironmentBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxVERTICAL );
	
	m_panelEnv = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer12->AddGrowableCol( 1 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText44 = new wxStaticText( m_panelEnv, wxID_ANY, _("Environment variable set to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	fgSizer12->Add( m_staticText44, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceEnvChoices;
	m_choiceEnv = new wxChoice( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEnvChoices, 0 );
	m_choiceEnv->SetSelection( 0 );
	fgSizer12->Add( m_choiceEnv, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText45 = new wxStaticText( m_panelEnv, wxID_ANY, _("Debugger 'PreDefined Types' set to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	fgSizer12->Add( m_staticText45, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceDbgEnvChoices;
	m_choiceDbgEnv = new wxChoice( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDbgEnvChoices, 0 );
	m_choiceDbgEnv->SetSelection( 0 );
	fgSizer12->Add( m_choiceDbgEnv, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer34->Add( fgSizer12, 0, wxEXPAND, 5 );
	
	m_staticline12 = new wxStaticLine( m_panelEnv, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer34->Add( m_staticline12, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText47 = new wxStaticText( m_panelEnv, wxID_ANY, _("Additional environment variables:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	bSizer34->Add( m_staticText47, 0, wxALL, 5 );
	
	m_textCtrlEnvvars = new wxTextCtrl( m_panelEnv, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlEnvvars->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer34->Add( m_textCtrlEnvvars, 1, wxALL|wxEXPAND, 5 );
	
	m_panelEnv->SetSizer( bSizer34 );
	m_panelEnv->Layout();
	bSizer34->Fit( m_panelEnv );
	bSizer44->Add( m_panelEnv, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer44 );
	this->Layout();
	bSizer44->Fit( this );
	
	// Connect Events
	m_choiceEnv->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
	m_choiceDbgEnv->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlEnvvars->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
}

PSEnvironmentBasePage::~PSEnvironmentBasePage()
{
	// Disconnect Events
	m_choiceEnv->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
	m_choiceDbgEnv->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlEnvvars->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSEnvironmentBasePage::OnCmdEvtVModified ), NULL, this );
}

PSBuildEventsBasePage::PSBuildEventsBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxVERTICAL );
	
	m_preBuildPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( m_preBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer8->Add( m_staticText11, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlBuildEvents = new wxTextCtrl( m_preBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlBuildEvents->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer9->Add( m_textCtrlBuildEvents, 1, wxALL|wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	m_preBuildPage->SetSizer( bSizer8 );
	m_preBuildPage->Layout();
	bSizer8->Fit( m_preBuildPage );
	bSizer41->Add( m_preBuildPage, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer41 );
	this->Layout();
	bSizer41->Fit( this );
}

PSBuildEventsBasePage::~PSBuildEventsBasePage()
{
}

PSCustomBuildBasePage::PSCustomBuildBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxVERTICAL );
	
	m_customBuildPage = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_checkEnableCustomBuild = new wxCheckBox( m_customBuildPage, wxID_ANY, _("Enable custom build"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_checkEnableCustomBuild, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline12 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer15->Add( m_staticline12, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText33 = new wxStaticText( m_customBuildPage, wxID_ANY, _("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	bSizer23->Add( m_staticText33, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCustomBuildWD = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer23->Add( m_textCtrlCustomBuildWD, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseCustomBuildWD = new wxButton( m_customBuildPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_buttonBrowseCustomBuildWD, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer15->Add( bSizer23, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer211;
	bSizer211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlTargets = new wxListCtrl( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer211->Add( m_listCtrlTargets, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer221;
	bSizer221 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonNewCustomTarget, 0, wxALL, 5 );
	
	m_buttonEditCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonEditCustomTarget, 0, wxALL, 5 );
	
	m_buttonDeleteCustomTarget = new wxButton( m_customBuildPage, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer221->Add( m_buttonDeleteCustomTarget, 0, wxALL, 5 );
	
	bSizer211->Add( bSizer221, 0, wxEXPAND, 5 );
	
	bSizer15->Add( bSizer211, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer191;
	bSizer191 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText29 = new wxStaticText( m_customBuildPage, wxID_ANY, _("Makefile Generators:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer191->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline11 = new wxStaticLine( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer191->Add( m_staticline11, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer15->Add( bSizer191, 0, 0, 5 );
	
	wxArrayString m_thirdPartyToolChoices;
	m_thirdPartyTool = new wxChoice( m_customBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_thirdPartyToolChoices, 0 );
	m_thirdPartyTool->SetSelection( 0 );
	bSizer15->Add( m_thirdPartyTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText30 = new wxStaticText( m_customBuildPage, wxID_ANY, _("Command to use for makefile generation:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer15->Add( m_staticText30, 0, wxALL, 5 );
	
	m_textCtrlMakefileGenerationCmd = new wxTextCtrl( m_customBuildPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	bSizer15->Add( m_textCtrlMakefileGenerationCmd, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_customBuildPage->SetSizer( bSizer15 );
	m_customBuildPage->Layout();
	bSizer15->Fit( m_customBuildPage );
	bSizer42->Add( m_customBuildPage, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer42 );
	this->Layout();
	bSizer42->Fit( this );
	
	// Connect Events
	m_checkEnableCustomBuild->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabled ), NULL, this );
	m_checkEnableCustomBuild->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildCBEnabledUI ), NULL, this );
	m_staticText33->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_textCtrlCustomBuildWD->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomBuildBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCustomBuildWD->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonBrowseCustomBuildWD->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnBrowseCustomBuildWD ), NULL, this );
	m_buttonBrowseCustomBuildWD->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( PSCustomBuildBasePage::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PSCustomBuildBasePage::OnItemSelected ), NULL, this );
	m_listCtrlTargets->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonNewCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnNewTarget ), NULL, this );
	m_buttonNewCustomTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonEditCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnEditTarget ), NULL, this );
	m_buttonEditCustomTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnEditTargetUI ), NULL, this );
	m_buttonDeleteCustomTarget->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnDeleteTarget ), NULL, this );
	m_buttonDeleteCustomTarget->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnDeleteTargetUI ), NULL, this );
	m_staticText29->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_thirdPartyTool->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSCustomBuildBasePage::OnChoiceMakefileTool ), NULL, this );
	m_thirdPartyTool->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_staticText30->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomBuildBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
}

PSCustomBuildBasePage::~PSCustomBuildBasePage()
{
	// Disconnect Events
	m_checkEnableCustomBuild->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabled ), NULL, this );
	m_checkEnableCustomBuild->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildCBEnabledUI ), NULL, this );
	m_staticText33->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_textCtrlCustomBuildWD->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomBuildBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCustomBuildWD->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonBrowseCustomBuildWD->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnBrowseCustomBuildWD ), NULL, this );
	m_buttonBrowseCustomBuildWD->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( PSCustomBuildBasePage::OnItemActivated ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( PSCustomBuildBasePage::OnItemSelected ), NULL, this );
	m_listCtrlTargets->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonNewCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnNewTarget ), NULL, this );
	m_buttonNewCustomTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_buttonEditCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnEditTarget ), NULL, this );
	m_buttonEditCustomTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnEditTargetUI ), NULL, this );
	m_buttonDeleteCustomTarget->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PSCustomBuildBasePage::OnDeleteTarget ), NULL, this );
	m_buttonDeleteCustomTarget->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnDeleteTargetUI ), NULL, this );
	m_staticText29->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_thirdPartyTool->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( PSCustomBuildBasePage::OnChoiceMakefileTool ), NULL, this );
	m_thirdPartyTool->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_staticText30->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomBuildBasePage::OnCmdEvtVModified ), NULL, this );
	m_textCtrlMakefileGenerationCmd->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomBuildBasePage::OnCustomBuildEnabledUI ), NULL, this );
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
	fgSizer10 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer10->AddGrowableCol( 1 );
	fgSizer10->SetFlexibleDirection( wxBOTH );
	fgSizer10->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, _("C++ Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer10->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer10->Add( m_textCompilerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonCompilerOptions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText46 = new wxStaticText( m_compilerPage, wxID_ANY, _("C Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText46->Wrap( -1 );
	fgSizer10->Add( m_staticText46, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCCompileOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer10->Add( m_textCtrlCCompileOptions, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonCCompileOptions = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonCCompileOptions, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, _("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer10->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer10->Add( m_textAdditionalSearchPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonAddSearchPath, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, _("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer10->Add( m_staticText171, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer10->Add( m_textPreprocessor, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer10->Add( m_buttonAddPreprocessor, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	compilerPageSizer->Add( fgSizer10, 0, wxEXPAND, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	m_notebook->AddPage( m_compilerPage, _("Compiler"), false );
	m_linkerPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 3, 3, 0, 0 );
	fgSizer11->AddGrowableCol( 1 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, _("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer11->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH );
	fgSizer11->Add( m_textLinkerOptions, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLinkerOptions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, _("Library Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer11->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH );
	fgSizer11->Add( m_textLibraryPath, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLibraryPath, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, _("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer11->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH );
	m_textLibraries->SetToolTip( _("Enter any extra library names, separated by';' e.g. Foo  or  Foo;Bar") );
	
	fgSizer11->Add( m_textLibraries, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer11->Add( m_buttonLibraries, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	linkerPageSizer->Add( fgSizer11, 0, wxEXPAND, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	m_notebook->AddPage( m_linkerPage, _("Linker"), true );
	m_resourceCmpPage = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText221 = new wxStaticText( m_resourceCmpPage, wxID_ANY, _("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText221->Wrap( -1 );
	fgSizer4->Add( m_staticText221, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpOptions = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpOptions, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpOptions = new wxButton( m_resourceCmpPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpOptions, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_staticText23 = new wxStaticText( m_resourceCmpPage, wxID_ANY, _("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer4->Add( m_staticText23, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAddResCmpPath = new wxTextCtrl( m_resourceCmpPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textAddResCmpPath, 0, wxEXPAND|wxALL, 5 );
	
	m_buttonAddResCmpPath = new wxButton( m_resourceCmpPage, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_buttonAddResCmpPath, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer151->Add( fgSizer4, 1, wxEXPAND, 5 );
	
	m_resourceCmpPage->SetSizer( bSizer151 );
	m_resourceCmpPage->Layout();
	bSizer151->Fit( m_resourceCmpPage );
	m_notebook->AddPage( m_resourceCmpPage, _("Resources"), false );
	
	bSizer117->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer117 );
	this->Layout();
	bSizer117->Fit( this );
	
	// Connect Events
	m_textCompilerOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCompilerOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddCompilerOptions ), NULL, this );
	m_textCtrlCCompileOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCCompileOptions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddCCompilerOptions ), NULL, this );
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
	m_textCtrlCCompileOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GlobalSettingsBasePanel::OnCmdEvtVModified ), NULL, this );
	m_buttonCCompileOptions->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GlobalSettingsBasePanel::OnButtonAddCCompilerOptions ), NULL, this );
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

PSCustomMakefileBasePage::PSCustomMakefileBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxVERTICAL );
	
	m_customMakefileStep = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->AddGrowableRow( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText25 = new wxStaticText( m_customMakefileStep, wxID_ANY, _("Dependencies:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer5->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textDeps = new wxTextCtrl( m_customMakefileStep, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL|wxTE_RICH2 );
	m_textDeps->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer5->Add( m_textDeps, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText26 = new wxStaticText( m_customMakefileStep, wxID_ANY, _("Rule action:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer5->Add( m_staticText26, 0, wxALL, 5 );
	
	m_textPreBuildRule = new wxTextCtrl( m_customMakefileStep, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textPreBuildRule->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer5->Add( m_textPreBuildRule, 0, wxALL|wxEXPAND, 5 );
	
	bSizer16->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_customMakefileStep, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText24 = new wxStaticText( m_customMakefileStep, wxID_ANY, _("Define here a custom makefile rule to be executed in the pre-build steps.\nSee the wiki for more help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	sbSizer2->Add( m_staticText24, 0, wxALL, 5 );
	
	bSizer16->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	m_customMakefileStep->SetSizer( bSizer16 );
	m_customMakefileStep->Layout();
	bSizer16->Fit( m_customMakefileStep );
	bSizer43->Add( m_customMakefileStep, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer43 );
	this->Layout();
	bSizer43->Fit( this );
	
	// Connect Events
	m_staticText25->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_textDeps->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomMakefileBasePage::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_staticText26->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_textPreBuildRule->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomMakefileBasePage::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_staticText24->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
}

PSCustomMakefileBasePage::~PSCustomMakefileBasePage()
{
	// Disconnect Events
	m_staticText25->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_textDeps->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomMakefileBasePage::OnCmdEvtVModified ), NULL, this );
	m_textDeps->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_staticText26->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_textPreBuildRule->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCustomMakefileBasePage::OnCmdEvtVModified ), NULL, this );
	m_textPreBuildRule->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
	m_staticText24->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PSCustomMakefileBasePage::OnProjectCustumBuildUI ), NULL, this );
}

PSCompletionBase::PSCompletionBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( PSCompletionBase::m_splitter1OnIdle ), NULL, this );
	m_panel14 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText47 = new wxStaticText( m_panel14, wxID_ANY, _("Search paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	bSizer35->Add( m_staticText47, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlSearchPaths = new wxTextCtrl( m_panel14, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH2 );
	m_textCtrlSearchPaths->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer35->Add( m_textCtrlSearchPaths, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_panel14->SetSizer( bSizer35 );
	m_panel14->Layout();
	bSizer35->Fit( m_panel14 );
	m_panel15 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter2 = new wxSplitterWindow( m_panel15, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter2->SetSashGravity( 0.5 );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( PSCompletionBase::m_splitter2OnIdle ), NULL, this );
	m_panel16 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText48 = new wxStaticText( m_panel16, wxID_ANY, _("Compile flags (clang only):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText48->Wrap( -1 );
	bSizer37->Add( m_staticText48, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlCmpOptions = new wxTextCtrl( m_panel16, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlCmpOptions->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer37->Add( m_textCtrlCmpOptions, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_panel16->SetSizer( bSizer37 );
	m_panel16->Layout();
	bSizer37->Fit( m_panel16 );
	m_panel17 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText49 = new wxStaticText( m_panel17, wxID_ANY, _("Macros (clang only):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText49->Wrap( -1 );
	bSizer38->Add( m_staticText49, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_textCtrlMacros = new wxTextCtrl( m_panel17, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlMacros->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer38->Add( m_textCtrlMacros, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	m_panel17->SetSizer( bSizer38 );
	m_panel17->Layout();
	bSizer38->Fit( m_panel17 );
	m_splitter2->SplitHorizontally( m_panel16, m_panel17, 0 );
	bSizer36->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer40;
	bSizer40 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxC11 = new wxCheckBox( m_panel15, wxID_ANY, _("Enable C++11 Standard"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer40->Add( m_checkBoxC11, 0, wxALL|wxEXPAND, 5 );
	
	bSizer36->Add( bSizer40, 0, wxEXPAND, 5 );
	
	m_panel15->SetSizer( bSizer36 );
	m_panel15->Layout();
	bSizer36->Fit( m_panel15 );
	m_splitter1->SplitHorizontally( m_panel14, m_panel15, 0 );
	bSizer34->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer34 );
	this->Layout();
	bSizer34->Fit( this );
	
	// Connect Events
	m_textCtrlSearchPaths->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCmpOptions->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlMacros->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxC11->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
}

PSCompletionBase::~PSCompletionBase()
{
	// Disconnect Events
	m_textCtrlSearchPaths->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlCmpOptions->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_textCtrlMacros->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
	m_checkBoxC11->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PSCompletionBase::OnCmdEvtVModified ), NULL, this );
}
