///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "syntaxhighlightbasedlg.h"

///////////////////////////////////////////////////////////////////////////

SyntaxHighlightBaseDlg::SyntaxHighlightBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonApply->SetToolTip( wxT("Apply the current changes to the editor without closing the dialog") );
	
	buttonSizer->Add( m_buttonApply, 0, wxALL, 5 );
	
	m_buttonDefaults = new wxButton( this, wxID_ANY, wxT("Defaults..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonDefaults->SetToolTip( wxT("Restore all lexers to default values") );
	
	buttonSizer->Add( m_buttonDefaults, 0, wxALL, 5 );
	
	bSizer1->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonCancel ), NULL, this );
	m_buttonApply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonApply ), NULL, this );
	m_buttonDefaults->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnRestoreDefaults ), NULL, this );
}

SyntaxHighlightBaseDlg::~SyntaxHighlightBaseDlg()
{
	// Disconnect Events
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonOK ), NULL, this );
	m_buttonCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonCancel ), NULL, this );
	m_buttonApply->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnButtonApply ), NULL, this );
	m_buttonDefaults->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SyntaxHighlightBaseDlg::OnRestoreDefaults ), NULL, this );
}
