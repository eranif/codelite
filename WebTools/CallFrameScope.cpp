#include "CallFrameScope.h"

CallFrameScope::CallFrameScope() {}

CallFrameScope::~CallFrameScope() {}

void CallFrameScope::FromJSON(const JSONElement& json)
{
    m_type = json.namedObject("type").toString();
    m_name = json.namedObject("name").toString();
    m_remoteObject.FromJSON(json.namedObject("object"));
}

JSONElement CallFrameScope::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("type", m_type);
    if(!m_name.IsEmpty()) { json.addProperty("name", m_name); }
    json.append(m_remoteObject.ToJSON("object"));
    return json;
}

wxString CallFrameScope::GetDisplayName() const
{
    if(GetName().IsEmpty()) {
        return GetType();
    } else {
        return GetName() + " : " + GetType();
    }
}
