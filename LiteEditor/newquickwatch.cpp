///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
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
	m_treeCtrl->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	bSizer5->Add( m_treeCtrl, 1, wxEXPAND, 0 );
	
	m_staticline1 = new wxStaticLine( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer5->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	m_panelExtra = new wxPanel( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_panelExtra, wxID_ANY, _("Hexadecimal:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 93, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT, 5 );
	
	m_hexFormat = new wxStaticText( m_panelExtra, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_hexFormat->Wrap( -1 );
	m_hexFormat->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_hexFormat, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( m_panelExtra, wxID_ANY, _("Binary:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 93, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT, 5 );
	
	m_binFormat = new wxStaticText( m_panelExtra, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_binFormat->Wrap( -1 );
	m_binFormat->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	fgSizer1->Add( m_binFormat, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_panelExtra->SetSizer( fgSizer1 );
	m_panelExtra->Layout();
	fgSizer1->Fit( m_panelExtra );
	bSizer5->Add( m_panelExtra, 0, wxEXPAND | wxALL, 5 );
	
	m_checkBoxShowMoreFormats = new wxCheckBox( m_mainPanel, wxID_ANY, _("Show selected node in Hex and Bin format"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_checkBoxShowMoreFormats, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
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
	m_checkBoxShowMoreFormats->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewQuickWatch::OnShowHexAndBinFormat ), NULL, this );
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
	m_checkBoxShowMoreFormats->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( NewQuickWatch::OnShowHexAndBinFormat ), NULL, this );
}
