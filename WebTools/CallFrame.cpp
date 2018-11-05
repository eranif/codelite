#include "CallFrame.h"

CallFrame::CallFrame() {}

CallFrame::~CallFrame() {}

JSONElement CallFrame::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("callFrameId", m_callFrameId);
    json.addProperty("functionName", m_functionName);
    json.append(m_location.ToJSON("location"));
    json.append(m_this.ToJSON("this"));
    return json;
}

void CallFrame::FromJSON(const JSONElement& json)
{
    m_callFrameId = json.namedObject("callFrameId").toString();
    m_functionName = json.namedObject("functionName").toString();
    m_location.FromJSON(json.namedObject("location"));
    m_this.FromJSON(json.namedObject("this"));
}
