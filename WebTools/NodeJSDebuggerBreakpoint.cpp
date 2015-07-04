#include "NodeJSDebuggerBreakpoint.h"

NodeJSBreakpoint::NodeJSBreakpoint()
    : m_line(wxNOT_FOUND)
    , m_nodeBpID(wxNOT_FOUND)
{
}

NodeJSBreakpoint::~NodeJSBreakpoint() {}

void NodeJSBreakpoint::FromJSON(const JSONElement& json)
{
    m_filename = json.namedObject("file").toString();
    m_line = json.namedObject("line").toInt();
}

JSONElement NodeJSBreakpoint::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("file", m_filename);
    json.addProperty("line", m_line);
    return json;
}
