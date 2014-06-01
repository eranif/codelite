#include "CompilerLocatorMSVC9.h"
#include <wx/filename.h>

CompilerLocatorMSVC9::CompilerLocatorMSVC9()
{
}

CompilerLocatorMSVC9::~CompilerLocatorMSVC9()
{
}

bool CompilerLocatorMSVC9::Locate()
{
    m_compilers.clear();
    
    // We locate it by searching for the environment variable
    // %VS90COMNTOOLS%
    // This is for Visual Studio 2008
    wxString path = ::wxGetenv("VS90COMNTOOLS");
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
    AddTools(masterPath.GetPath(), "Visual C++ 9");
    return true;
}
