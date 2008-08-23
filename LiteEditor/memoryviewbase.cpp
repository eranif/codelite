///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "memoryviewbase.h"

///////////////////////////////////////////////////////////////////////////

MemoryViewBase::MemoryViewBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Address:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_textCtrlExpression = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textCtrlExpression, 1, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_textCtrlSize = new wxTextCtrl( this, wxID_ANY, wxT("32"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_textCtrlSize, 0, wxALL, 5 );
	
	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlMemory = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlMemory->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer3->Add( m_textCtrlMemory, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonEvaluate = new wxButton( this, wxID_ANY, wxT("Evaluate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonEvaluate->SetDefault(); 
	bSizer4->Add( m_buttonEvaluate, 0, wxALL, 5 );
	
	m_buttonUpdate = new wxButton( this, wxID_ANY, wxT("Update"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonUpdate, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 0, 0, 5 );
	
	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	m_textCtrlExpression->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MemoryViewBase::OnTextEntered ), NULL, this );
	m_textCtrlMemory->Connect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MemoryViewBase::OnTextDClick ), NULL, this );
	m_buttonEvaluate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryViewBase::OnEvaluate ), NULL, this );
	m_buttonEvaluate->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MemoryViewBase::OnEvaluateUI ), NULL, this );
	m_buttonUpdate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryViewBase::OnUpdate ), NULL, this );
}

MemoryViewBase::~MemoryViewBase()
{
	// Disconnect Events
	m_textCtrlExpression->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( MemoryViewBase::OnTextEntered ), NULL, this );
	m_textCtrlMemory->Disconnect( wxEVT_LEFT_DCLICK, wxMouseEventHandler( MemoryViewBase::OnTextDClick ), NULL, this );
	m_buttonEvaluate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryViewBase::OnEvaluate ), NULL, this );
	m_buttonEvaluate->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MemoryViewBase::OnEvaluateUI ), NULL, this );
	m_buttonUpdate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryViewBase::OnUpdate ), NULL, this );
}
