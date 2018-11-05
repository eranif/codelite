#include "Location.h"

Location::~Location() {}

JSONElement Location::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("lineNumber", m_lineNumber);
    json.addProperty("scriptId", m_scriptId);
    return json;
}

void Location::FromJSON(const JSONElement& json)
{
    m_lineNumber = json.namedObject("lineNumber").toInt(0);
    m_scriptId = json.namedObject("scriptId").toString();
}
