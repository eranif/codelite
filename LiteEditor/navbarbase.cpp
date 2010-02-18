///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "navbarbase.h"

///////////////////////////////////////////////////////////////////////////

NavBarBase::NavBarBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter->SetSashGravity( 0 );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( NavBarBase::m_splitterOnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_scopeChoices;
	m_scope = new wxChoice( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_scopeChoices, 0 );
	m_scope->SetSelection( 0 );
	bSizer3->Add( m_scope, 0, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_funcChoices;
	m_func = new wxChoice( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_funcChoices, 0 );
	m_func->SetSelection( 0 );
	bSizer4->Add( m_func, 0, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_splitter->SplitVertically( m_panel1, m_panel2, 150 );
	mainSizer->Add( m_splitter, 0, wxEXPAND|wxALL, 2 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_scope->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NavBarBase::OnScope ), NULL, this );
	m_scope->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NavBarBase::OnScopeListMouseDown ), NULL, this );
	m_func->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NavBarBase::OnFunction ), NULL, this );
	m_func->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NavBarBase::OnFuncListMouseDown ), NULL, this );
}

NavBarBase::~NavBarBase()
{
	// Disconnect Events
	m_scope->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NavBarBase::OnScope ), NULL, this );
	m_scope->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NavBarBase::OnScopeListMouseDown ), NULL, this );
	m_func->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( NavBarBase::OnFunction ), NULL, this );
	m_func->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NavBarBase::OnFuncListMouseDown ), NULL, this );
}
