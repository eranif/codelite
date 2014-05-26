#include "CompilerLocatorMSVC11.h"
#include <wx/filename.h>

CompilerLocatorMSVC11::CompilerLocatorMSVC11()
{
}

CompilerLocatorMSVC11::~CompilerLocatorMSVC11()
{
}

bool CompilerLocatorMSVC11::Locate()
{
    m_compilers.clear();
    
    // We locate it by searching for the environment variable
    // %VS120COMNTOOLS%
    wxString path = ::wxGetenv("VS110COMNTOOLS");
    if ( path.IsEmpty() ) {
        return false;
    }
    
    // We found the installation path
    // Go to 1 folder up
    wxFileName masterPath(path, "");
    wxFileName binPath( masterPath );
    binPath.RemoveLastDir();
    binPath.RemoveLastDir();
    binPath.AppendDir("VC");
    binPath.AppendDir("bin");
    
    wxFileName cl(binPath.GetPath(), "cl.exe");
    if ( !cl.FileExists() ) {
        return false;
    }
    
    // Add the tools
    AddTools(masterPath.GetPath(), "Visual C++ 11");
    return true;
}
