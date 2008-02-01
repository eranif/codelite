///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep 26 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cscopetabbase.h"

///////////////////////////////////////////////////////////////////////////

CscopeTabBase::CscopeTabBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_statusMessage = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_statusMessage->Wrap( -1 );
	bSizer2->Add( m_statusMessage, 1, wxEXPAND|wxTOP|wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonClear = new wxButton( this, wxID_ANY, wxT("&Clear Results"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonClear, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline2, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_treeCtrlResults = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	mainSizer->Add( m_treeCtrlResults, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,15 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetValue( 0 ); 
	mainSizer->Add( m_gauge, 0, wxALL|wxEXPAND, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CscopeTabBase::OnClearResults ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
}
