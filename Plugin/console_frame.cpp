///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/app.h>
#include "console_frame.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "plugin.h"
#include "windowattrmanager.h"
#include "globals.h"
#include "imanager.h"

///////////////////////////////////////////////////////////////////////////

ConsoleFrame::ConsoleFrame( wxWindow* parent, IManager* manager, wxWindowID id )
    : wxPanel( parent, id, wxDefaultPosition, wxSize(300, 200))
    , m_manager(manager)
{
    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    SetBackgroundColour(DrawingUtils::GetOutputPaneBgColour());
    m_terminal = new wxTerminal(this);
    bSizer1->Add(m_terminal, 1, wxEXPAND|wxALL);

    this->SetSizer( bSizer1 );
    this->Layout();

    // Connect Events
    EventNotifier::Get()->Connect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
}

ConsoleFrame::~ConsoleFrame()
{
    // Disconnect Events
    EventNotifier::Get()->Disconnect(wxEVT_DEBUG_ENDED, wxCommandEventHandler(ConsoleFrame::OnDebuggerEnded), NULL, this);
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
    m_manager->GetDockingManager()->DetachPane(this);
    Destroy();
    m_manager->GetDockingManager()->Update();
#endif
    e.Skip();
}
