///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/app.h>
#include "console_frame.h"
#include "drawingutils.h"
#include "plugin.h"
#include "windowattrmanager.h"
#include "globals.h"
#include "frame.h"

///////////////////////////////////////////////////////////////////////////

ConsoleFrame::ConsoleFrame( wxWindow* parent, wxWindowID id )
: wxPanel( parent, id, wxDefaultPosition, wxSize(300, 200))
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
	m_terminal = new wxTerminal(this);
	bSizer1->Add(m_terminal, 1, wxEXPAND|wxALL);

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	wxTheApp->Connect(wxEVT_DEBUG_ENDED,               wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
	wxTheApp->Connect(wxEVT_DEBUG_EDITOR_GOT_CONTROL,  wxCommandEventHandler(ConsoleFrame::OnEditorGotControl), NULL, this);
	wxTheApp->Connect(wxEVT_DEBUG_EDITOR_LOST_CONTROL, wxCommandEventHandler(ConsoleFrame::OnEditorLostControl), NULL, this);
}

ConsoleFrame::~ConsoleFrame()
{
	// Disconnect Events
	wxTheApp->Disconnect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
	wxTheApp->Disconnect(wxEVT_DEBUG_EDITOR_GOT_CONTROL,  wxCommandEventHandler(ConsoleFrame::OnEditorGotControl), NULL, this);
	wxTheApp->Disconnect(wxEVT_DEBUG_EDITOR_LOST_CONTROL, wxCommandEventHandler(ConsoleFrame::OnEditorLostControl), NULL, this);
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

void ConsoleFrame::OnDebuggerEnded(wxCommandEvent& e)
{
#ifndef __WXMSW__

	// Save the debug perspective
	wxString debugPrespective(wxStandardPaths::Get().GetUserDataDir() + wxT("/config/debug.layout"));
	WriteFileWithBackup(debugPrespective, clMainFrame::Get()->GetDockingManager().SavePerspective(), false);

	clMainFrame::Get()->GetDockingManager().DetachPane(this);
	Destroy();
	clMainFrame::Get()->GetDockingManager().Update();
#endif
	e.Skip();
}

void ConsoleFrame::OnEditorGotControl(wxCommandEvent& e)
{
	LEditor *editor = clMainFrame::Get()->GetMainBook()->GetActiveEditor();
	if(editor) {
		editor->SetActive();
	}
	e.Skip();
}

void ConsoleFrame::OnEditorLostControl(wxCommandEvent& e)
{
	e.Skip();
	Raise();
	m_terminal->SetFocus();
}

