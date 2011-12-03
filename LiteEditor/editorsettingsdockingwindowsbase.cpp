///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsdockingwindowsbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsDockingWindowsBase::EditorSettingsDockingWindowsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Output Pane:") ), wxVERTICAL );
	
	m_checkBoxHideOutputPaneOnClick = new wxCheckBox( this, wxID_ANY, _("When user clicks inside an editor, hide the output pane -- unless it's one of:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneOnClick->SetValue(true); 
	m_checkBoxHideOutputPaneOnClick->SetToolTip( _("When e.g. you compile your project, or use 'Find in Files', the Output Pane opens to show the results. If this box is ticked, it will automatically close as soon as you click in the editor.") );
	
	sbSizer1->Add( m_checkBoxHideOutputPaneOnClick, 0, wxEXPAND|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxHideOutputPaneNotIfBuild = new wxCheckBox( this, wxID_ANY, _("Build"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfBuild->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfBuild, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfErrors = new wxCheckBox( this, wxID_ANY, _("Errors"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfErrors->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfErrors, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfSearch = new wxCheckBox( this, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfSearch->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfSearch, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfReplace = new wxCheckBox( this, wxID_ANY, _("Replace"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfReplace->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfReplace, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfReferences = new wxCheckBox( this, wxID_ANY, _("References"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfReferences->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfReferences, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfOutput = new wxCheckBox( this, wxID_ANY, _("Output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfOutput->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfOutput, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfDebug = new wxCheckBox( this, wxID_ANY, _("Debug"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfDebug->SetValue(true); 
	m_checkBoxHideOutputPaneNotIfDebug->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfDebug, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfTrace = new wxCheckBox( this, wxID_ANY, _("Trace"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfTrace->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfTrace, 0, wxALL, 5 );
	
	m_checkBoxHideOutputPaneNotIfTasks = new wxCheckBox( this, wxID_ANY, _("Tasks"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfTasks->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	fgSizer1->Add( m_checkBoxHideOutputPaneNotIfTasks, 0, wxALL, 5 );
	
	sbSizer1->Add( fgSizer1, 0, wxALIGN_LEFT|wxEXPAND|wxRIGHT|wxLEFT, 20 );
	
	m_checkBoxDontFoldSearchResults = new wxCheckBox( this, wxID_ANY, _("Don't automatically fold Search results"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxDontFoldSearchResults->SetToolTip( _("By default, all but the first results of 'Search' are automatically folded; you have to click on each subsequent file to see its contained matches. Tick this box to prevent this.\nYou can still fold and unfold results with the button in the output pane toolbar. ") );
	
	sbSizer1->Add( m_checkBoxDontFoldSearchResults, 0, wxALL, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Find Bar:") ), wxVERTICAL );
	
	m_checkBoxFindBarAtBottom = new wxCheckBox( this, wxID_ANY, _("Place the find bar at the bottom"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxFindBarAtBottom, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Debug Pane:") ), wxVERTICAL );
	
	m_checkBoxShowDebugOnRun = new wxCheckBox( this, wxID_ANY, _("Show 'Debug' tab on Start Debugger"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxShowDebugOnRun->SetValue(true); 
	m_checkBoxShowDebugOnRun->SetToolTip( _("When starting the debugger, if the Debug tab is not visible, checking this will make it visible") );
	
	sbSizer11->Add( m_checkBoxShowDebugOnRun, 0, wxALL, 5 );
	
	mainSizer->Add( sbSizer11, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_radioBoxHintChoices[] = { _("Transparent hint"), _("Rectangle hint"), _("Venetian blinds hints") };
	int m_radioBoxHintNChoices = sizeof( m_radioBoxHintChoices ) / sizeof( wxString );
	m_radioBoxHint = new wxRadioBox( this, wxID_ANY, _("Docking Style:"), wxDefaultPosition, wxDefaultSize, m_radioBoxHintNChoices, m_radioBoxHintChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxHint->SetSelection( 0 );
	bSizer2->Add( m_radioBoxHint, 1, wxALL|wxEXPAND, 5 );
	
	wxString m_radioBoxTabControlStyleChoices[] = { _("Classic"), _("Glossy"), _("Curved") };
	int m_radioBoxTabControlStyleNChoices = sizeof( m_radioBoxTabControlStyleChoices ) / sizeof( wxString );
	m_radioBoxTabControlStyle = new wxRadioBox( this, wxID_ANY, _("Tab Control Style:"), wxDefaultPosition, wxDefaultSize, m_radioBoxTabControlStyleNChoices, m_radioBoxTabControlStyleChoices, 1, wxRA_SPECIFY_COLS );
	m_radioBoxTabControlStyle->SetSelection( 0 );
	bSizer2->Add( m_radioBoxTabControlStyle, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_checkBoxHideOutputPaneNotIfBuild->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfErrors->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfSearch->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfReplace->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfReferences->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfOutput->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfTrace->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfTasks->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

EditorSettingsDockingWindowsBase::~EditorSettingsDockingWindowsBase()
{
	// Disconnect Events
	m_checkBoxHideOutputPaneNotIfBuild->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfErrors->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfSearch->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfReplace->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfReferences->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfOutput->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfDebug->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfTrace->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	m_checkBoxHideOutputPaneNotIfTasks->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
	
}
