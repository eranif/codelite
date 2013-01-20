///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingscommentsbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsCommentsBase::EditorSettingsCommentsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxSmartAddFiles = new wxCheckBox( this, wxID_ANY, _("When adding new files to a project, place the files in the 'include' / 'src' folders respectively"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_checkBoxSmartAddFiles, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_checkBoxContCComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C style comment section automatically add '*' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxContCComment->SetValue(true); 
	bSizer2->Add( m_checkBoxContCComment, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxContinueCppComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C++ style comment section, automatically add '//' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_checkBoxContinueCppComment, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Code navigation key:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceNavKeyChoices[] = { _("Shift"), _("Control"), _("Alt") };
	int m_choiceNavKeyNChoices = sizeof( m_choiceNavKeyChoices ) / sizeof( wxString );
	m_choiceNavKey = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceNavKeyNChoices, m_choiceNavKeyChoices, 0 );
	m_choiceNavKey->SetSelection( 0 );
	m_choiceNavKey->SetToolTip( _("When using quick code navigation use this key in combination with mouse click\nTo quickly go to implementation / declaration") );
	
	bSizer3->Add( m_choiceNavKey, 1, wxALL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCommentsBase::~EditorSettingsCommentsBase()
{
}
