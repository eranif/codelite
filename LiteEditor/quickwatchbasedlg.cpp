///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "quickwatchbasedlg.h"

///////////////////////////////////////////////////////////////////////////

QuickWatchBaseDlg::QuickWatchBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl1 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_textCtrl1, 0, wxALL|wxEXPAND, 5 );
	
	m_localVarsTree = new LocalVarsTree(m_panel1, wxID_ANY);
	bSizer3->Add( m_localVarsTree, 1, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonEvaluate = new wxButton( this, wxID_ANY, wxT("&Evaluate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonEvaluate->SetDefault(); 
	bSizer2->Add( m_buttonEvaluate, 0, wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonClose, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_buttonEvaluate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickWatchBaseDlg::OnReEvaluate ), NULL, this );
	m_buttonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickWatchBaseDlg::OnButtonClose ), NULL, this );
}

QuickWatchBaseDlg::~QuickWatchBaseDlg()
{
	// Disconnect Events
	m_buttonEvaluate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickWatchBaseDlg::OnReEvaluate ), NULL, this );
	m_buttonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickWatchBaseDlg::OnButtonClose ), NULL, this );
}
