#include "PHPUserWorkspace.h"
#include <globals.h>
#include "JSON.h"

PHPUserWorkspace::PHPUserWorkspace(const wxString& workspacePath)
    : m_workspacePath(workspacePath)
{
}

PHPUserWorkspace::~PHPUserWorkspace()
{
}

wxFileName PHPUserWorkspace::GetFileName() const
{
    wxFileName workspaceFile( m_workspacePath );
    wxFileName fn(workspaceFile.GetPath(), workspaceFile.GetFullName() + "." + ::clGetUserName());
    fn.AppendDir(".codelite");
    if ( !fn.FileExists() ) {
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    return fn;
}

PHPUserWorkspace& PHPUserWorkspace::Load()
{
    wxFileName fn = GetFileName();
    JSON root(fn);
    JSONItem element = root.toElement();
    m_breakpoints.clear();
    
    JSONItem bpArr = element.namedObject("m_breakpoints");
    int bpcount = bpArr.arraySize();
    for( int i=0; i<bpcount; ++i ) {
        XDebugBreakpoint bp;
        bp.FromJSON( bpArr.arrayItem(i) );
        m_breakpoints.push_back( bp );
    }
    return *this;
}

PHPUserWorkspace& PHPUserWorkspace::Save()
{
    // Serialize the breakpoints
    JSON root(cJSON_Object);
    JSONItem json = root.toElement();
    JSONItem bpArr = JSONItem::createArray("m_breakpoints");
    json.append( bpArr );
    
    XDebugBreakpoint::List_t::const_iterator iter = m_breakpoints.begin();
    for( ; iter != m_breakpoints.end(); ++iter ) {
        bpArr.arrayAppend( iter->ToJSON() );
    }
    root.save( GetFileName() );
    return *this;
}
