#include "wxTerminalEvent.hpp"

wxTerminalEvent::wxTerminalEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
{
}

wxEvent* wxTerminalEvent::Clone() const { return new wxTerminalEvent(*this); }

// A specialization of MyProcess for redirecting the output
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_READY, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_STDERR, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_DONE, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalEvent);
