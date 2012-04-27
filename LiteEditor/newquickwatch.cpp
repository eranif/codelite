///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	m_treeCtrl = new wxTreeCtrl( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_EDIT_LABELS|wxTR_SINGLE|wxNO_BORDER );
	m_treeCtrl->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	m_treeCtrl->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	bSizer5->Add( m_treeCtrl, 1, wxEXPAND, 0 );
	
	m_mainPanel->SetSizer( bSizer5 );
	m_mainPanel->Layout();
	bSizer5->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_treeCtrl->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NewQuickWatch::OnLeftDown ), NULL, this );
	m_treeCtrl->Connect( wxEVT_MOTION, wxMouseEventHandler( NewQuickWatch::OnMouseMove ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( NewQuickWatch::OnEditLabelStart ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( NewQuickWatch::OnEditLabelEnd ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( NewQuickWatch::OnItemExpanded ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( NewQuickWatch::OnItemMenu ), NULL, this );
}

NewQuickWatch::~NewQuickWatch()
{
	// Disconnect Events
	m_treeCtrl->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( NewQuickWatch::OnKeyDown ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( NewQuickWatch::OnLeftDown ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_MOTION, wxMouseEventHandler( NewQuickWatch::OnMouseMove ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( NewQuickWatch::OnEditLabelStart ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( NewQuickWatch::OnEditLabelEnd ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( NewQuickWatch::OnItemExpanded ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEventHandler( NewQuickWatch::OnExpandItem ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( NewQuickWatch::OnItemMenu ), NULL, this );
}
