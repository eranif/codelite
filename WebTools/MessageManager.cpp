#include "DebuggerPaused.h"
#include "DebuggerResumed.h"
#include "DebuggerScriptParsed.h"
#include "MessageManager.h"
#include "RuntimeExecutionContextDestroyed.h"

MessageManager::MessageManager()
{
    AddHandler(NodeMessageBase::Ptr_t(new DebuggerPaused()));
    AddHandler(NodeMessageBase::Ptr_t(new DebuggerScriptParsed()));
    AddHandler(NodeMessageBase::Ptr_t(new DebuggerResumed()));
    AddHandler(NodeMessageBase::Ptr_t(new RuntimeExecutionContextDestroyed()));
}

MessageManager::~MessageManager() {}

void MessageManager::AddHandler(NodeMessageBase::Ptr_t handler)
{
    m_events.insert({ handler->GetEventName(), handler });
}

NodeMessageBase::Ptr_t MessageManager::GetHandler(const wxString& eventName)
{
    if(m_events.count(eventName) == 0) { return NodeMessageBase::Ptr_t(nullptr); }
    return m_events[eventName]->Clone();
}
