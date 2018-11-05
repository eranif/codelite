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

public:
    CallFrame();
    virtual ~CallFrame();
    JSONElement ToJSON(const wxString& name) const;
    void FromJSON(const JSONElement& json);

    void SetCallFrameId(const wxString& callFrameId) { this->m_callFrameId = callFrameId; }
    void SetFunctionName(const wxString& functionName) { this->m_functionName = functionName; }
    void SetLocation(const Location& location) { this->m_location = location; }
    void SetThis(const RemoteObject& This) { this->m_this = This; }
    const wxString& GetCallFrameId() const { return m_callFrameId; }
    const wxString& GetFunctionName() const { return m_functionName; }
    const Location& GetLocation() const { return m_location; }
    const RemoteObject& GetThis() const { return m_this; }
};

#endif // NODEDBGCALLFRAME_H
