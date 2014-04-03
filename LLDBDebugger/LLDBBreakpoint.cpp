#include "LLDBBreakpoint.h"
#include "json_node.h"
#include <wx/filename.h>
#include "workspace.h"

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

LLDBBreakpoint::LLDBBreakpoint(const LLDBBreakpoint& other)
{
    *this = other;
}

bool LLDBBreakpoint::operator==(const LLDBBreakpoint& other) const 
{
    return  m_type == other.m_type &&
            m_name == other.m_name &&
            m_filename == other.m_filename &&
            m_lineNumber == other.m_lineNumber;
}

LLDBBreakpoint& LLDBBreakpoint::operator=(const LLDBBreakpoint& other)
{
    if ( &other == this ) 
        return *this;

    m_id = other.m_id;
    m_type = other.m_type;
    m_name = other.m_name;
    m_filename = other.m_filename;
    m_lineNumber = other.m_lineNumber;
    return *this;
}

LLDBBreakpoint::Vec_t LLDBBreakpoint::FromBreakpointInfoVector(const BreakpointInfo::Vec_t& breakpoints)
{
    LLDBBreakpoint::Vec_t bps;
    for(size_t i=0; i<breakpoints.size(); ++i) {
        if ( breakpoints.at(i).bp_type == BP_type_break ) {
            LLDBBreakpoint bp;
            const BreakpointInfo &gdbBp = breakpoints.at(i);
            bp.SetName( gdbBp.function_name );
            bp.SetFilename( gdbBp.file );
            bp.SetLineNumber( gdbBp.lineno );
            bp.SetType( gdbBp.function_name.IsEmpty() ? kFileLine : kFunction );
            bps.push_back( bp );
        }
    }
    return bps;
}

BreakpointInfo::Vec_t LLDBBreakpoint::ToBreakpointInfoVector(const LLDBBreakpoint::Vec_t& breakpoints)
{
    BreakpointInfo::Vec_t bps;
    for(size_t i=0; i<breakpoints.size(); ++i) {
        const LLDBBreakpoint &bp = breakpoints.at(i);
        BreakpointInfo gdbBp;
        gdbBp.function_name = bp.GetName();
        gdbBp.lineno = bp.GetLineNumber();
        gdbBp.file = bp.GetFilename();
        gdbBp.bp_type = BP_type_break;
        bps.push_back( gdbBp );
    }
    return bps;
}

LLDBBreakpoint::Vec_t LLDBBreakpoint::LoadFromDisk()
{
    if ( !WorkspaceST::Get()->IsOpen() ) {
        return LLDBBreakpoint::Vec_t();
    }
    
    LLDBBreakpoint::Vec_t breakpoints;
    wxFileName fn = GetBreakpointsConfigFile();
    JSONRoot root(fn);
    JSONElement arr = root.toElement();
    for(int i=0; i<arr.arraySize(); ++i) {
        LLDBBreakpoint bp;
        bp.FromJSON( arr.arrayItem(i) );
        breakpoints.push_back( bp );
    }
    return breakpoints;
}

void LLDBBreakpoint::SaveToDisk(const LLDBBreakpoint::Vec_t &breakpoints)
{
    if ( !WorkspaceST::Get()->IsOpen() ) {
        return;
    }

    wxFileName fn = GetBreakpointsConfigFile();
    JSONRoot root(cJSON_Array);
    for(size_t i=0; i<breakpoints.size(); ++i) {
        LLDBBreakpoint bp = breakpoints.at(i); // We do a copy breakpoint here and not a reference since we want to invalidate it before saving to disk
        bp.Invalidate();
        root.toElement().append( bp.ToJSON() );
    }
    root.save( fn );
}

wxFileName LLDBBreakpoint::GetBreakpointsConfigFile()
{
    if ( !WorkspaceST::Get()->IsOpen() ) {
        return wxFileName();
    }
    
    wxFileName fn(WorkspaceST::Get()->GetPrivateFolder(), "lldb-breakpoints.conf");
    return fn;
}

void LLDBBreakpoint::FromJSON(const JSONElement& element)
{
    m_id = element.namedObject("m_id").toInt(wxNOT_FOUND);
    m_type = element.namedObject("m_type").toInt(kFileLine);
    m_name = element.namedObject("m_name").toString();
    m_filename = element.namedObject("m_filename").toString();
    m_lineNumber = element.namedObject("m_lineNumber").toInt(wxNOT_FOUND);
}

JSONElement LLDBBreakpoint::ToJSON() const
{
    JSONElement element = JSONElement::createObject();
    element.addProperty("m_id", m_id);
    element.addProperty("m_type", m_type);
    element.addProperty("m_name", m_name);
    element.addProperty("m_filename", m_filename);
    element.addProperty("m_lineNumber", m_lineNumber);
    return element;
}

wxString LLDBBreakpoint::ToString() const
{
    wxString str;
    if ( GetType() == kFileLine ) {
        str << "Normal breakpoint. " << GetFilename() << "," << GetLineNumber();
    } else if ( GetType() == kFunction ) {
        str << "Normal breakpoint. " << GetName() << "()";
    }
    
    return str;
}
