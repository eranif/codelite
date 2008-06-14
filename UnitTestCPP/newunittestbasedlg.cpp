///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newunittestbasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewUnitTestBaseDlg::NewUnitTestBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText30 = new wxStaticText( this, wxID_ANY, wxT("Test Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	fgSizer6->Add( m_staticText30, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlTestName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlTestName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxUseFixture = new wxCheckBox( this, wxID_ANY, wxT("Use fixture"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxUseFixture->SetValue(true);
	
	fgSizer6->Add( m_checkBoxUseFixture, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_filler = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	fgSizer6->Add( m_filler, 1, wxEXPAND|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText32 = new wxStaticText( this, wxID_ANY, wxT("Fixture Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer6->Add( m_staticText32, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlFixtureName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlFixtureName, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer19->Add( fgSizer6, 1, wxEXPAND|wxALL, 5 );
	
	m_staticline11 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer19->Add( m_staticline11, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button24 = new wxButton( this, wxID_OK, wxT("&Create"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button24->SetDefault(); 
	bSizer20->Add( m_button24, 0, wxALL, 5 );
	
	m_button23 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button23, 0, wxALL, 5 );
	
	bSizer19->Add( bSizer20, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkBoxUseFixture->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewUnitTestBaseDlg::OnUseFixture ), NULL, this );
}

NewUnitTestBaseDlg::~NewUnitTestBaseDlg()
{
	// Disconnect Events
	m_checkBoxUseFixture->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewUnitTestBaseDlg::OnUseFixture ), NULL, this );
}
