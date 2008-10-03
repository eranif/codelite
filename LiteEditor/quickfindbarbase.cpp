///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "quickfindbarbase.h"

///////////////////////////////////////////////////////////////////////////

QuickFindBarBase::QuickFindBarBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonClose = new wxButton( this, wxID_ANY, wxT("X"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer2->Add( m_buttonClose, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlFindWhat = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_textCtrlFindWhat->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOBK ) );
	m_textCtrlFindWhat->SetMinSize( wxSize( 200,-1 ) );
	
	bSizer2->Add( m_textCtrlFindWhat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_buttonNext = new wxButton( this, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonNext->SetDefault(); 
	bSizer2->Add( m_buttonNext, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonPrevious = new wxButton( this, wxID_ANY, wxT("Previous"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonPrevious, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_checkBoxMatchCase = new wxCheckBox( this, wxID_ANY, wxT("Match case"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer2->Add( m_checkBoxMatchCase, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	mainSizer->Add( bSizer2, 1, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 0 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_buttonClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonClose ), NULL, this );
	m_textCtrlFindWhat->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_textCtrlFindWhat->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( QuickFindBarBase::OnText ), NULL, this );
	m_textCtrlFindWhat->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnButtonNext ), NULL, this );
	m_buttonNext->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonNext ), NULL, this );
	m_buttonPrevious->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonPrevious ), NULL, this );
}

QuickFindBarBase::~QuickFindBarBase()
{
	// Disconnect Events
	m_buttonClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonClose ), NULL, this );
	m_textCtrlFindWhat->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( QuickFindBarBase::OnKeyDown ), NULL, this );
	m_textCtrlFindWhat->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( QuickFindBarBase::OnText ), NULL, this );
	m_textCtrlFindWhat->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( QuickFindBarBase::OnButtonNext ), NULL, this );
	m_buttonNext->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonNext ), NULL, this );
	m_buttonPrevious->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( QuickFindBarBase::OnButtonPrevious ), NULL, this );
}
