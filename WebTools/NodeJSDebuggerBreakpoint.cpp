#include "NodeFileManager.h"
#include "NodeJSDebuggerBreakpoint.h"

NodeJSBreakpoint::NodeJSBreakpoint() {}

NodeJSBreakpoint::~NodeJSBreakpoint() {}

void NodeJSBreakpoint::FromJSON(const JSONElement& json)
{
    m_filename = json.namedObject("url").toString();
    m_line = json.namedObject("lineNumber").toInt();
}

JSONElement NodeJSBreakpoint::ToJSON(const wxString& name) const
{
    JSONElement json = JSONElement::createObject(name);
    json.addProperty("url", NodeFileManager::FileNameToURI(m_filename));
    json.addProperty("lineNumber", m_line);
    return json;
}
