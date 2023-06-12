#include "wxTerminalEvent.hpp"

wxTerminalEvent::wxTerminalEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
    , m_answer(false)
    , m_allowed(true)
    , m_lineNumber(0)
    , m_selected(false)
{
}

wxTerminalEvent::wxTerminalEvent(const wxTerminalEvent& event)
    : wxCommandEvent(event)
    , m_answer(false)
    , m_allowed(true)
{
    *this = event;
}

wxTerminalEvent& wxTerminalEvent::operator=(const wxTerminalEvent& src)
{
    m_strings.clear();
    for(size_t i = 0; i < src.m_strings.size(); ++i) {
        m_strings.Add(src.m_strings.Item(i).c_str());
    }
    m_fileName = src.m_fileName;
    m_answer = src.m_answer;
    m_allowed = src.m_allowed;
    m_oldName = src.m_oldName;
    m_lineNumber = src.m_lineNumber;
    m_selected = src.m_selected;
    m_stringRaw = src.m_stringRaw;

    // Copy wxCommandEvent members here
    m_eventType = src.m_eventType;
    m_id = src.m_id;
    m_cmdString = src.m_cmdString;
    m_commandInt = src.m_commandInt;
    m_extraLong = src.m_extraLong;
    return *this;
}

wxTerminalEvent::~wxTerminalEvent() {}

wxEvent* wxTerminalEvent::Clone() const { return new wxTerminalEvent(*this); }

// A specialization of MyProcess for redirecting the output
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_READY, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_OUTPUT, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_STDERR, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_DONE, wxTerminalEvent);
wxDEFINE_EVENT(wxEVT_TERMINAL_CTRL_SET_TITLE, wxTerminalEvent);
