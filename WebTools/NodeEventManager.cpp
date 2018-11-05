#include "DebuggerPaused.h"
#include "MessageManager.h"

NodeEventManager::NodeEventManager() { AddHandler(NodeMessageBase::Ptr_t(new DebuggerPaused())); }

NodeEventManager::~NodeEventManager() {}

void NodeEventManager::AddHandler(NodeMessageBase::Ptr_t handler)
{
    m_events.insert({ handler->GetEventName(), handler });
}

NodeMessageBase::Ptr_t NodeEventManager::GetHandler(const wxString& eventName)
{
    if(m_events.count(eventName) == 0) { return NodeMessageBase::Ptr_t(nullptr); }
    return m_events[eventName]->Clone();
}
