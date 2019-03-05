#ifndef NODEDBGCALLFRAME_H
#define NODEDBGCALLFRAME_H

#include "Location.h"
#include "RemoteObject.h"
#include "nSerializableObject.h"

class CallFrame : public nSerializableObject
{
    wxString m_callFrameId;
    wxString m_functionName;
    Location m_location;
    RemoteObject m_this;
    nSerializableObject::Vec_t m_scopeChain;

public:
    CallFrame();
    virtual ~CallFrame();
    JSONItem ToJSON(const wxString& name) const;
    void FromJSON(const JSONItem& json);

    void SetCallFrameId(const wxString& callFrameId) { this->m_callFrameId = callFrameId; }
    void SetFunctionName(const wxString& functionName) { this->m_functionName = functionName; }
    void SetLocation(const Location& location) { this->m_location = location; }
    void SetThis(const RemoteObject& This) { this->m_this = This; }
    const wxString& GetCallFrameId() const { return m_callFrameId; }
    const wxString& GetFunctionName() const { return m_functionName; }
    const Location& GetLocation() const { return m_location; }
    const RemoteObject& GetThis() const { return m_this; }
    void SetScopeChain(const nSerializableObject::Vec_t& scopeChain) { this->m_scopeChain = scopeChain; }
    const nSerializableObject::Vec_t& GetScopeChain() const { return m_scopeChain; }
};

#endif // NODEDBGCALLFRAME_H
