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
	
	m_checkBoxContCComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C style comment section,\nautomatically add '*' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxContCComment->SetValue(true); 
	bSizer2->Add( m_checkBoxContCComment, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxContinueCppComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C++ style comment section,\nautomatically add '//' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_checkBoxContinueCppComment, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCommentsBase::~EditorSettingsCommentsBase()
{
}
