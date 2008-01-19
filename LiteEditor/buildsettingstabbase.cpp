///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "buildsettingstabbase.h"

///////////////////////////////////////////////////////////////////////////

BuildTabSettingsBase::BuildTabSettingsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Background colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	m_staticText4->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Compiler errors colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPickerError = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 98, 98 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerError, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Compiler warnings colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPickerWarnings = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 255, 170 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourPickerWarnings, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxSkipeWarnings = new wxCheckBox( this, wxID_ANY, wxT("When using the menu to jump to errors, skip warnings"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer1->Add( m_checkBoxSkipeWarnings, 0, wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
}
