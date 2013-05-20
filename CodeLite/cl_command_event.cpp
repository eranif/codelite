#include "cl_command_event.h"

clCommandEvent::clCommandEvent(wxEventType commandType, int winid)
    : wxCommandEvent(commandType, winid)
{
}

clCommandEvent::clCommandEvent(const clCommandEvent& event)
    : wxCommandEvent(event)
{
    m_ptr = event.m_ptr;
}

clCommandEvent::~clCommandEvent()
{
    m_ptr.reset();
}

wxEvent* clCommandEvent::Clone() const
{
    clCommandEvent *new_event = new clCommandEvent(*this);
    return new_event;
}

void clCommandEvent::SetClientObject(wxClientData* clientObject)
{
    m_ptr = clientObject;
}

wxClientData* clCommandEvent::GetClientObject() const
{
    return m_ptr.get();
}
