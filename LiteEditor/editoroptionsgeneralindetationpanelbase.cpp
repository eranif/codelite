///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editoroptionsgeneralindetationpanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralIndetationPanelBase::EditorOptionsGeneralIndetationPanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_indentsUsesTabs = new wxCheckBox( this, wxID_ANY, _("Use tabs in indentation"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer1->Add( m_indentsUsesTabs, 0, wxALL, 5 );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Columns per indentation level:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_indentWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	fgSizer1->Add( m_indentWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Columns per tab character in document:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_tabWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 20, 4 );
	fgSizer1->Add( m_tabWidth, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	bSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
}

EditorOptionsGeneralIndetationPanelBase::~EditorOptionsGeneralIndetationPanelBase()
{
}
