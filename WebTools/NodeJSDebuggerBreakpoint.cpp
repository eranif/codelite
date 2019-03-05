#include "NodeFileManager.h"
#include "NodeJSDebuggerBreakpoint.h"

NodeJSBreakpoint::NodeJSBreakpoint() {}

NodeJSBreakpoint::~NodeJSBreakpoint() {}

void NodeJSBreakpoint::FromJSON(const JSONItem& json)
{
    m_filename = json.namedObject("url").toString();
    m_filename = NodeFileManager::URIToFileName(m_filename);
    m_line = json.namedObject("lineNumber").toInt();
}

JSONItem NodeJSBreakpoint::ToJSON(const wxString& name) const
{
    JSONItem json = JSONItem::createObject(name);
    json.addProperty("url", NodeFileManager::FileNameToURI(m_filename));
    json.addProperty("lineNumber", m_line);
    return json;
}
