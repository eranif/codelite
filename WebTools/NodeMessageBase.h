#ifndef NODEDBGEVENTBASE_H
#define NODEDBGEVENTBASE_H

#include "JSON.h"
#include "SocketAPI/clWebSocketClient.h"

#include <memory>

class NodeMessageBase
{
public:
    using Ptr_t = std::shared_ptr<NodeMessageBase>;

protected:
    wxString m_eventName;

public:
    NodeMessageBase(const wxString& eventName);
    virtual ~NodeMessageBase() = default;
    /**
     * @brief process message in JSON format
     */
    virtual void Process(clWebSocketClient& socket, const JSONItem& json) = 0;

    /**
     * @brief create new instance of this type
     */
    virtual NodeMessageBase::Ptr_t Clone() = 0;

    void SetEventName(const wxString& eventName) { this->m_eventName = eventName; }
    const wxString& GetEventName() const { return m_eventName; }
};

#endif // NODEDBGEVENTBASE_H
