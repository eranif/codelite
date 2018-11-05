#ifndef NODEEVENTMANAGER_H
#define NODEEVENTMANAGER_H

#include "NodeMessageBase.h"
#include "wxStringHash.h"

class MessageManager
{
    std::unordered_map<wxString, NodeMessageBase::Ptr_t> m_events;

protected:
    void AddHandler(NodeMessageBase::Ptr_t handler);

public:
    MessageManager();
    virtual ~MessageManager();
    NodeMessageBase::Ptr_t GetHandler(const wxString& eventName);
};

#endif // NODEEVENTMANAGER_H
