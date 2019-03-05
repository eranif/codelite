#include "Location.h"

Location::~Location() {}

JSONItem Location::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("lineNumber", m_lineNumber);
    json.addProperty("scriptId", m_scriptId);
    return json;
}

void Location::FromJSON(const JSONItem& json)
{
    m_lineNumber = json.namedObject("lineNumber").toInt(0);
    m_scriptId = json.namedObject("scriptId").toString();
}
