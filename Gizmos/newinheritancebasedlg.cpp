///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newinheritancebasedlg.h"

///////////////////////////////////////////////////////////////////////////

NewIneritanceBaseDlg::NewIneritanceBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Parent class:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer5->Add( m_staticText2, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlInhertiance = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlInhertiance->SetMinSize( wxSize( 300,-1 ) );
	
	bSizer2->Add( m_textCtrlInhertiance, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonMore = new wxButton( m_panel1, wxID_ANY, wxT("More..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer2->Add( m_buttonMore, 0, wxALL, 5 );
	
	bSizer5->Add( bSizer2, 0, wxEXPAND, 0 );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Inheritance access:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL, 5 );
	
	wxString m_choiceAccessChoices[] = { wxT("public"), wxT("private"), wxT("protected") };
	int m_choiceAccessNChoices = sizeof( m_choiceAccessChoices ) / sizeof( wxString );
	m_choiceAccess = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceAccessNChoices, m_choiceAccessChoices, 0 );
	m_choiceAccess->SetSelection( 0 );
	bSizer5->Add( m_choiceAccess, 0, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer5 );
	m_panel1->Layout();
	bSizer5->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 0, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonMore->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewIneritanceBaseDlg::OnButtonMore ), NULL, this );
}

NewIneritanceBaseDlg::~NewIneritanceBaseDlg()
{
	// Disconnect Events
	m_buttonMore->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( NewIneritanceBaseDlg::OnButtonMore ), NULL, this );
}
