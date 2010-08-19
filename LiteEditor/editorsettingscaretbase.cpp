///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingscaretbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsCaretBase::EditorSettingsCaretBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Caret blink period (milliseconds):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlBlinkPeriod = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000, 500 );
	m_spinCtrlBlinkPeriod->SetToolTip( _("Set the caret blink period in milliseconds. Setting the period to 0 stops the caret blinking ") );
	
	fgSizer1->Add( m_spinCtrlBlinkPeriod, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Caret width (pixels):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlCaretWidth = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 1 );
	fgSizer1->Add( m_spinCtrlCaretWidth, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_checkBoxCaretUseCamelCase = new wxCheckBox( this, wxID_ANY, _("Caret jumps between word segments marked by capitalisation (CamelCase) or undescores"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_checkBoxCaretUseCamelCase, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCaretBase::~EditorSettingsCaretBase()
{
}
