#include "LLDBBreakpoint.h"

LLDBBreakpoint::~LLDBBreakpoint()
{
}

LLDBBreakpoint::LLDBBreakpoint(const wxFileName& filename, int line)
    : m_id(wxNOT_FOUND)
    , m_type(kFileLine)
    , m_filename(filename.GetFullPath())
    , m_lineNumber(line)
{
}

LLDBBreakpoint::LLDBBreakpoint(const wxString& name)
    : m_id(wxNOT_FOUND)
    , m_type(kFunction)
    , m_name(name)
    , m_lineNumber(wxNOT_FOUND)
{
}

bool LLDBBreakpoint::operator==(const LLDBBreakpoint& other) const 
{
    return  m_id == other.m_id &&
            m_type == other.m_type &&
            m_name == other.m_name &&
            m_filename == other.m_filename &&
            m_lineNumber == other.m_lineNumber;
}
