///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "messagepane_base.h"

///////////////////////////////////////////////////////////////////////////

MessagePaneBase::MessagePaneBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel2->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1 = new wxStaticBitmap( m_panel2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,32 ), 0 );
	bSizer4->Add( m_bitmap1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_panel1 = new wxPanel( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxWANTS_CHARS );
	m_panel1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Message:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	m_staticText2->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	m_staticText2->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	bSizer3->Add( m_staticText2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticTextMessage = new wxStaticText( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMessage->Wrap( -1 );
	m_staticTextMessage->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );
	m_staticTextMessage->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	bSizer3->Add( m_staticTextMessage, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer4->Add( m_panel1, 1, wxEXPAND|wxALL, 5 );
	
	m_bpButtonClose = new wxBitmapButton( m_panel2, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButtonClose->SetDefault(); 
	bSizer4->Add( m_bpButtonClose, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mainSizer->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_staticline1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonAction = new wxButton( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAction->SetDefault(); 
	bSizer5->Add( m_buttonAction, 0, wxALL, 5 );
	
	m_buttonAction1 = new wxButton( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonAction1, 0, wxALL, 5 );
	
	m_buttonAction2 = new wxButton( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_buttonAction2, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_panel2->SetSizer( mainSizer );
	m_panel2->Layout();
	mainSizer->Fit( m_panel2 );
	bSizer6->Add( m_panel2, 1, wxEXPAND | wxALL, 2 );
	
	this->SetSizer( bSizer6 );
	this->Layout();
	bSizer6->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( MessagePaneBase::OnEraseBG ) );
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( MessagePaneBase::OnPaint ) );
	m_panel1->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MessagePaneBase::OnKeyDown ), NULL, this );
	m_bpButtonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnButtonClose ), NULL, this );
	m_buttonAction->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton ), NULL, this );
	m_buttonAction1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton1 ), NULL, this );
	m_buttonAction2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton2 ), NULL, this );
}

MessagePaneBase::~MessagePaneBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( MessagePaneBase::OnEraseBG ) );
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( MessagePaneBase::OnPaint ) );
	m_panel1->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MessagePaneBase::OnKeyDown ), NULL, this );
	m_bpButtonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnButtonClose ), NULL, this );
	m_buttonAction->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton ), NULL, this );
	m_buttonAction1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton1 ), NULL, this );
	m_buttonAction2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton2 ), NULL, this );
}
