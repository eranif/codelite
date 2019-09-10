#include "csCommandHandlerBase.h"
#include "csManager.h"
wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSED, clCommandEvent);

csCommandHandlerBase::csCommandHandlerBase(csManager* manager)
    : m_manager(manager)
    , m_notifyOnExit(true)
{
}

csCommandHandlerBase::~csCommandHandlerBase() {}

void csCommandHandlerBase::NotifyCompletion()
{
    clCommandEvent e(wxEVT_COMMAND_PROCESSED);
    m_manager->AddPendingEvent(e);
}

void csCommandHandlerBase::Process(const JSONItem& options)
{
    DoProcessCommand(options);
    if(m_notifyOnExit) {
        // Make sure we call 'NotifyCompletion' here if needed
        NotifyCompletion();
    }
}
