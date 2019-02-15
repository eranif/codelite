#include "NodeJSLocator.h"
#include "cl_standard_paths.h"
#include "globals.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/volume.h>
#endif

NodeJSLocator::NodeJSLocator() {}

NodeJSLocator::~NodeJSLocator() {}

void NodeJSLocator::Locate(const wxArrayString& hints)
{
    wxArrayString paths = hints;
#if defined(__WXGTK__) || defined(__WXOSX__)
    // Linux
    
    // add the standard paths
    paths.Add("/usr/local/bin");
    paths.Add("/usr/bin");

    wxFileName nodejs;
    wxFileName npm;
    if(TryPaths(paths, "node", nodejs) || TryPaths(paths, "nodejs", nodejs) || TryPaths(paths, "node.osx", nodejs)) {
        m_nodejs = nodejs.GetFullPath();
    }

    if(TryPaths(paths, "npm", npm)) {
        m_npm = npm.GetFullPath();
    }

#elif defined(__WXMSW__)
    // Registry paths are searched first
    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\Node.js
        wxRegKey regKeyNodeJS(wxRegKey::HKLM, "SOFTWARE\\Node.js");
        wxString clInstallFolder;
        if(regKeyNodeJS.Exists() && regKeyNodeJS.QueryValue("InstallPath", clInstallFolder) &&
           wxDirExists(clInstallFolder)) {
            paths.Add(clInstallFolder);
        }
    }

    {
        // HKEY_LOCAL_MACHINE\SOFTWARE\Node.js
        wxRegKey regKeyNodeJS(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Node.js");
        wxString clInstallFolder;
        if(regKeyNodeJS.Exists() && regKeyNodeJS.QueryValue("InstallPath", clInstallFolder) &&
           wxDirExists(clInstallFolder)) {
            paths.Add(clInstallFolder);
        }
    }
    
    // Still could not find it, try the PATH environment variable
    wxFileName fn_node;
    if(::clFindExecutable("node", fn_node, paths)) {
        m_nodejs = fn_node.GetFullPath();
    }
    
    wxFileName fn_npm;
    // On Windows, first try to locate npm.cmd
    if(m_npm.IsEmpty() && ::clFindExecutable("npm.cmd", fn_npm, paths)) {
        m_npm = fn_npm.GetFullPath();
    }
    
    // No luck? locate npm (with any given extension...)
    if(m_npm.IsEmpty() && ::clFindExecutable("npm", fn_npm, paths)) {
        m_npm = fn_npm.GetFullPath();
    }
#endif
}

bool NodeJSLocator::TryPaths(const wxArrayString& paths, const wxString& fullname, wxFileName& fullpath)
{
    for(size_t i = 0; i < paths.size(); ++i) {
        wxFileName fn(paths.Item(i), fullname);
        if(fn.FileExists()) {
            fullpath = fn;
            return true;
        }
    }
    return false;
}
