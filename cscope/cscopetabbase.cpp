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
	
	m_treeCtrlResults = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT );
	mainSizer->Add( m_treeCtrlResults, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_treeCtrlResults->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( CscopeTabBase::OnLeftDClick ), NULL, this );
	m_treeCtrlResults->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( CscopeTabBase::OnItemActivated ), NULL, this );
}
