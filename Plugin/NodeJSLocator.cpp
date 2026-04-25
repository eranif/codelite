#include "NodeJSLocator.h"

#include "fileutils.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

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

    // On Windows, first try to locate npm.cmd
    if (m_npm.IsEmpty()) {
        m_npm = ::FileUtils::FindExe("npm.cmd", paths).value_or(wxFileName{}).GetFullPath();
    }
#endif

    // Still could not find it, try the PATH environment variable
    if (m_nodejs.empty()) {
        m_nodejs = ::FileUtils::FindExe("node", paths).value_or(wxFileName{}).GetFullPath();
    }

    // No luck? locate npm
    if (m_npm.empty()) {
        m_npm = ::FileUtils::FindExe("npm", paths).value_or(wxFileName{}).GetFullPath();
    }
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
