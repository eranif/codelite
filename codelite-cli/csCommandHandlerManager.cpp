#include "csCommandHandlerManager.h"

csCommandHandlerManager::csCommandHandlerManager() {}

csCommandHandlerManager::~csCommandHandlerManager() {}

void csCommandHandlerManager::Register(const wxString& command, csCommandHandlerBase::Ptr_t handler)
{
    std::unordered_map<wxString, csCommandHandlerBase::Ptr_t>::iterator iter = m_handlers.find(command);
    if(iter != m_handlers.end()) { m_handlers.erase(iter); }
    m_handlers[command] = handler;
}

csCommandHandlerBase::Ptr_t csCommandHandlerManager::FindHandler(const wxString& command) const
{
    std::unordered_map<wxString, csCommandHandlerBase::Ptr_t>::const_iterator iter = m_handlers.find(command);
    if(iter == m_handlers.end()) { return csCommandHandlerBase::Ptr_t(nullptr); }
    return iter->second;
}
