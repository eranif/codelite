#ifndef NODEEVENTMANAGER_H
#define NODEEVENTMANAGER_H

#include "NodeDbgEventBase.h"
#include "wxStringHash.h"

class NodeEventManager
{
    std::unordered_map<wxString, NodeDbgEventBase::Ptr_t> m_events;

protected:
    void AddHandler(NodeDbgEventBase::Ptr_t handler);

public:
    NodeEventManager();
    virtual ~NodeEventManager();
    NodeDbgEventBase::Ptr_t GetHandler(const wxString& eventName);
};

#endif // NODEEVENTMANAGER_H
