///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
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

	m_checkBoxContCComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C style comment section,\nautomatically add '*' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxContCComment->SetValue(true);

	mainSizer->Add( m_checkBoxContCComment, 0, wxALL|wxEXPAND, 5 );

	m_checkBoxContinueCppComment = new wxCheckBox( this, wxID_ANY, _("When hitting <ENTER> in a C++ style comment section,\nautomatically add '//' at the next line"), wxDefaultPosition, wxDefaultSize, 0 );

	mainSizer->Add( m_checkBoxContinueCppComment, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCommentsBase::~EditorSettingsCommentsBase()
{
}
