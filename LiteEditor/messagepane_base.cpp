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
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( 32,32 ), 0 );
	bSizer4->Add( m_bitmap1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxWANTS_CHARS );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Message:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer3->Add( m_staticText2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticTextMessage = new wxStaticText( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextMessage->Wrap( -1 );
	bSizer3->Add( m_staticTextMessage, 0, wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer4->Add( m_panel1, 1, wxEXPAND|wxALL, 5 );
	
	m_bpButtonClose = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpButtonClose->SetDefault(); 
	bSizer4->Add( m_bpButtonClose, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mainSizer->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAction = new wxButton( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAction->SetDefault(); 
	bSizer5->Add( m_buttonAction, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_panel1->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( MessagePaneBase::OnKeyDown ), NULL, this );
	m_bpButtonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnButtonClose ), NULL, this );
	m_buttonAction->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton ), NULL, this );
}

MessagePaneBase::~MessagePaneBase()
{
	// Disconnect Events
	m_panel1->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( MessagePaneBase::OnKeyDown ), NULL, this );
	m_bpButtonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnButtonClose ), NULL, this );
	m_buttonAction->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MessagePaneBase::OnActionButton ), NULL, this );
}
