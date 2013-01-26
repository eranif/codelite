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
	m_staticText1->SetToolTip( _("When using quick code navigation use this key in combination with mouse click\nTo quickly go to implementation / declaration") );
	
	bSizer3->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Mouse Left Click +"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );
	
	m_checkBoxShift = new wxCheckBox( this, wxID_ANY, _("Shift"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxShift->SetValue(true); 
	bSizer3->Add( m_checkBoxShift, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxCtrl = new wxCheckBox( this, wxID_ANY, _("Ctrl"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_checkBoxCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxAlt = new wxCheckBox( this, wxID_ANY, _("Alt"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_checkBoxAlt, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer2->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCommentsBase::~EditorSettingsCommentsBase()
{
}
