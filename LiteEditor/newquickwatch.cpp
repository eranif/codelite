///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
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
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl = new wxTreeCtrl( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_SINGLE|wxNO_BORDER );
	m_treeCtrl->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	m_treeCtrl->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	bSizer5->Add( m_treeCtrl, 1, wxEXPAND, 0 );
	
	m_staticline1 = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer5->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	m_mainPanel->SetSizer( bSizer5 );
	m_mainPanel->Layout();
	bSizer5->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_treeCtrl->Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( NewQuickWatch::OnEnterWindow ), NULL, this );
	m_treeCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( NewQuickWatch::OnLeaveWindow ), NULL, this );
	m_treeCtrl->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NewQuickWatch::OnLeftDown ), NULL, this );
	m_treeCtrl->Connect( wxEVT_MOTION, wxMouseEventHandler( NewQuickWatch::OnMouseMove ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( NewQuickWatch::OnItemExpanded ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( NewQuickWatch::OnItemMenu ), NULL, this );
}

NewQuickWatch::~NewQuickWatch()
{
	// Disconnect Events
	m_treeCtrl->Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( NewQuickWatch::OnEnterWindow ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( NewQuickWatch::OnLeaveWindow ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NewQuickWatch::OnLeftDown ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_MOTION, wxMouseEventHandler( NewQuickWatch::OnMouseMove ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( NewQuickWatch::OnItemExpanded ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( NewQuickWatch::OnItemMenu ), NULL, this );
	
}
