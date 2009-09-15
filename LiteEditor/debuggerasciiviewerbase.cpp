///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "debuggerasciiviewerbase.h"

///////////////////////////////////////////////////////////////////////////

DebuggerAsciiViewerBase::DebuggerAsciiViewerBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlDbgCommand = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textCtrlDbgCommand->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	bSizer2->Add( m_textCtrlDbgCommand, 0, wxALL, 5 );
	
	m_textCtrlExpression = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_RICH2 );
	bSizer2->Add( m_textCtrlExpression, 1, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_textView = new wxScintilla(this);
	mainSizer->Add( m_textView, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_textCtrlExpression->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebuggerAsciiViewerBase::OnEnter ), NULL, this );
}

DebuggerAsciiViewerBase::~DebuggerAsciiViewerBase()
{
	// Disconnect Events
	m_textCtrlExpression->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DebuggerAsciiViewerBase::OnEnter ), NULL, this );
}
