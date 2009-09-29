///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	sbSizer1->Add( m_checkBoxHideOutputPaneOnClick, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Output Pane Control Bar:") ), wxVERTICAL );
	
	m_checkBoxShowQuickFinder = new wxCheckBox( this, wxID_ANY, _("Enable Quick Finder bar"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkBoxShowQuickFinder, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsDockingWindowsBase::~EditorSettingsDockingWindowsBase()
{
}
