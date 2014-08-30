#include "XDebugBreakpoint.h"

XDebugBreakpoint::XDebugBreakpoint(const wxString &filename, int line)
    : m_fileName(filename)
    , m_line(line)
    , m_breakpointId(wxNOT_FOUND)
{
}

XDebugBreakpoint::XDebugBreakpoint()
    : m_line(wxNOT_FOUND)
    , m_breakpointId(wxNOT_FOUND)
{
}

XDebugBreakpoint::~XDebugBreakpoint()
{
}

bool XDebugBreakpoint::operator==(const XDebugBreakpoint& other) const
{
    return this->m_fileName == other.m_fileName && this->m_line == other.m_line;
}

void XDebugBreakpoint::FromJSON(const JSONElement& json)
{
    m_fileName = json.namedObject("m_fileName").toString();
    m_line = json.namedObject("m_line").toInt(wxNOT_FOUND);
}

JSONElement XDebugBreakpoint::ToJSON() const
{
    JSONElement json = JSONElement::createObject();
    json.addProperty("m_fileName", m_fileName);
    json.addProperty("m_line", m_line);
    return json;
}
