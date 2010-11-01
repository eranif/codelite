///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/app.h>
#include "console_frame.h"
#include "plugin.h"
#include "windowattrmanager.h"

///////////////////////////////////////////////////////////////////////////

ConsoleFrame::ConsoleFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: wxFrame( parent, id, title, pos, size, style|wxFRAME_FLOAT_ON_PARENT )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_terminal = new wxTerminal(this);
	bSizer1->Add(m_terminal, 1, wxEXPAND|wxALL);

	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	WindowAttrManager::Load(this, wxT("console_frame"), NULL);

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW,                 wxCloseEventHandler( ConsoleFrame::OnClose ) );
	wxTheApp->Connect(wxEVT_DEBUG_ENDED,               wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
	wxTheApp->Connect(wxEVT_DEBUG_EDITOR_GOT_CONTROL,  wxCommandEventHandler(ConsoleFrame::OnEditorGotControl), NULL, this);
	wxTheApp->Connect(wxEVT_DEBUG_EDITOR_LOST_CONTROL, wxCommandEventHandler(ConsoleFrame::OnEditorLostControl), NULL, this);
}

ConsoleFrame::~ConsoleFrame()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ConsoleFrame::OnClose ) );
	wxTheApp->Disconnect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
	wxTheApp->Disconnect(wxEVT_DEBUG_EDITOR_GOT_CONTROL,  wxCommandEventHandler(ConsoleFrame::OnEditorGotControl), NULL, this);
	wxTheApp->Disconnect(wxEVT_DEBUG_EDITOR_LOST_CONTROL, wxCommandEventHandler(ConsoleFrame::OnEditorLostControl), NULL, this);
	WindowAttrManager::Save(this, wxT("console_frame"), NULL);

}

wxString ConsoleFrame::StartTTY()
{
#ifndef __WXMSW__
	wxString tty = m_terminal->StartTTY();
	return tty;
#else
	return wxT("");
#endif
}

void ConsoleFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
}

void ConsoleFrame::OnDebuggerEnded(wxCommandEvent& e)
{
#ifndef __WXMSW__
	this->Close();
#endif
}

void ConsoleFrame::OnEditorGotControl(wxCommandEvent& e)
{
	e.Skip();
}

void ConsoleFrame::OnEditorLostControl(wxCommandEvent& e)
{
	e.Skip();
	Raise();
	m_terminal->SetFocus();
}

