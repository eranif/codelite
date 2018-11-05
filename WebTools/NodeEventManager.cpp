#include "NodeEventDebuggerPaused.h"
#include "NodeEventManager.h"

NodeEventManager::NodeEventManager() { AddHandler(NodeDbgEventBase::Ptr_t(new NodeEventDebuggerPaused())); }

NodeEventManager::~NodeEventManager() {}

void NodeEventManager::AddHandler(NodeDbgEventBase::Ptr_t handler)
{
    m_events.insert({ handler->GetEventName(), handler });
}

NodeDbgEventBase::Ptr_t NodeEventManager::GetHandler(const wxString& eventName)
{
    if(m_events.count(eventName) == 0) { return NodeDbgEventBase::Ptr_t(nullptr); }
    return m_events[eventName]->Clone();
}
