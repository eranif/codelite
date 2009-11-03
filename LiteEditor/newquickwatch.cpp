///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "newquickwatch.h"

///////////////////////////////////////////////////////////////////////////

NewQuickWatch::NewQuickWatch( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_ROW_LINES|wxTR_SINGLE );
	mainSizer->Add( m_treeCtrl, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NewQuickWatch::OnCloseEvent ) );
	m_treeCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
}

NewQuickWatch::~NewQuickWatch()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( NewQuickWatch::OnCloseEvent ) );
	m_treeCtrl->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
}
