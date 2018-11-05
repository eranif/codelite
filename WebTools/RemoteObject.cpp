#include "RemoteObject.h"

RemoteObject::RemoteObject() {}

RemoteObject::~RemoteObject() {}

void RemoteObject::FromJSON(const JSONElement& json)
{
    m_type = json.namedObject("type").toString();
    m_subtype = json.namedObject("subtype").toString();
    m_className = json.namedObject("className").toString();
    m_value = json.namedObject("value").toString();
}

JSONElement RemoteObject::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("type", m_type);
    json.addProperty("subtype", m_subtype);
    json.addProperty("className", m_className);
    json.addProperty("value", m_value);
    return json;
}
