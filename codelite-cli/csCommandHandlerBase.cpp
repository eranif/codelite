#include "csCommandHandlerBase.h"
wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSED, clCommandEvent);

csCommandHandlerBase::csCommandHandlerBase(wxEvtHandler* sink)
    : m_sink(sink)
{
}

csCommandHandlerBase::~csCommandHandlerBase() {}

void csCommandHandlerBase::NotifyCompletion()
{
    clCommandEvent e(wxEVT_COMMAND_PROCESSED);
    m_sink->AddPendingEvent(e);
}
