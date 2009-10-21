///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
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
	
	m_checkBoxHideOutputPaneOnClick = new wxCheckBox( this, wxID_ANY, _("When user clicks inside an editor, hide the output pane"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneOnClick->SetToolTip( _("When e.g. you compile your project, or use 'Find in Files', the Output Pane opens to show the results. If this box is ticked, it will automatically close as soon as you click in the editor.") );
	
	sbSizer1->Add( m_checkBoxHideOutputPaneOnClick, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxHideOutputPaneNotIfDebug = new wxCheckBox( this, wxID_ANY, _("...unless it's the 'Debug' tab showing"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxHideOutputPaneNotIfDebug->SetValue(true); 
	m_checkBoxHideOutputPaneNotIfDebug->SetToolTip( _("Don't automatically close the Output Pane on an editor click if it's the Debug tab that's showing. You probably don't want it to close whenever you set a breakpoint, for example.") );
	
	bSizer2->Add( m_checkBoxHideOutputPaneNotIfDebug, 0, wxALL, 5 );
	
	sbSizer1->Add( bSizer2, 0, wxLEFT, 25 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Output Pane Control Bar:") ), wxVERTICAL );
	
	m_checkBoxShowQuickFinder = new wxCheckBox( this, wxID_ANY, _("Enable Quick Finder bar"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkBoxShowQuickFinder, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_checkBoxHideOutputPaneNotIfDebug->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}

EditorSettingsDockingWindowsBase::~EditorSettingsDockingWindowsBase()
{
	// Disconnect Events
	m_checkBoxHideOutputPaneNotIfDebug->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsDockingWindowsBase::OnHideOutputPaneNotIfDebugUI ), NULL, this );
}
