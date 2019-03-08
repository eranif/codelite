#include "LSPEvent.h"

wxDEFINE_EVENT(wxEVT_LSP_DEFINITION, LSPEvent);

LSPEvent::LSPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

LSPEvent::LSPEvent(const LSPEvent& src)
    : clCommandEvent(src)
{
    *this = src;
}

LSPEvent& LSPEvent::operator=(const LSPEvent& other)
{
    clCommandEvent::operator=(other);
    m_location = other.m_location;
    return *this;
}

LSPEvent::~LSPEvent() {}
