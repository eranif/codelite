#include "wxTerminalEvent.hpp"

wxTerminalCtrlEvent::wxTerminalCtrlEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
{
}

wxEvent* wxTerminalCtrlEvent::Clone() const { return new wxTerminalCtrlEvent(*this); }

// A specialization of MyProcess for redirecting the output
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_READY, wxTerminalCtrlEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalCtrlEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_STDERR, wxTerminalCtrlEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_DONE, wxTerminalCtrlEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalCtrlEvent);
