///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 10 2012)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uicallgraph.h"

///////////////////////////////////////////////////////////////////////////

uicallgraph::uicallgraph( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( m_panel3, wxID_ANY, _("Node threshold [%] :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer4->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_spinNT = new wxSpinCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	bSizer4->Add( m_spinNT, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_staticText2 = new wxStaticText( m_panel3, wxID_ANY, _("Edge threshold [%] :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );
	
	m_spinET = new wxSpinCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 60,-1 ), wxSP_ARROW_KEYS, 0, 100, 0 );
	bSizer4->Add( m_spinET, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 5 );
	
	m_checkBoxSN = new wxCheckBox( m_panel3, wxID_ANY, _("Short names"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_checkBoxSN, 0, wxRIGHT|wxLEFT|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_buttonRefresh = new wxButton( m_panel3, wxID_ANY, _("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonRefresh, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	
	m_panel3->SetSizer( bSizer4 );
	m_panel3->Layout();
	bSizer4->Fit( m_panel3 );
	bSizer1->Add( m_panel3, 0, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( uicallgraph::m_splitterOnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_scrolledWindow = new wxScrolledWindow( m_panel1, wxID_ANY, wxPoint( -1,-1 ), wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow->SetScrollRate( 5, 5 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Save call graph to...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );
	
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu1, wxID_ANY, wxString( _("Close call graph") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem2 );
	
	m_scrolledWindow->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( uicallgraph::m_scrolledWindowOnContextMenu ), NULL, this ); 
	
	bSizer2->Add( m_scrolledWindow, 1, wxEXPAND|wxLEFT, 5 );
	
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_grid = new wxGrid( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid->CreateGrid( 0, 4 );
	m_grid->EnableEditing( false );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );
	
	// Columns
	m_grid->AutoSizeColumns();
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 30 );
	m_grid->SetColLabelValue( 0, _(" Function name ") );
	m_grid->SetColLabelValue( 1, _(" Total time [%] ") );
	m_grid->SetColLabelValue( 2, _(" Self time [s]") );
	m_grid->SetColLabelValue( 3, _(" Called ") );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid->AutoSizeRows();
	m_grid->EnableDragRowSize( false );
	m_grid->SetRowLabelSize( 80 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer3->Add( m_grid, 0, wxALL|wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	m_splitter->SplitHorizontally( m_panel1, m_panel2, 400 );
	bSizer1->Add( m_splitter, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_buttonRefresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uicallgraph::OnRefreshClick ), NULL, this );
	m_scrolledWindow->Connect( wxEVT_PAINT, wxPaintEventHandler( uicallgraph::OnPaint ), NULL, this );
	this->Connect( m_menuItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( uicallgraph::OnSaveCallGraph ) );
	this->Connect( m_menuItem2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( uicallgraph::OnClosePanel ) );
}

uicallgraph::~uicallgraph()
{
	// Disconnect Events
	m_buttonRefresh->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( uicallgraph::OnRefreshClick ), NULL, this );
	m_scrolledWindow->Disconnect( wxEVT_PAINT, wxPaintEventHandler( uicallgraph::OnPaint ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( uicallgraph::OnSaveCallGraph ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( uicallgraph::OnClosePanel ) );
	
	delete m_menu1; 
}
