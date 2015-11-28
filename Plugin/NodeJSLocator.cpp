#include "NodeJSLocator.h"
#include "cl_standard_paths.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include <wx/volume.h>
#endif

NodeJSLocator::NodeJSLocator() {}

NodeJSLocator::~NodeJSLocator() {}

void NodeJSLocator::Locate()
{
#if defined(__WXGTK__) || defined(__WXOSX__)
    // Linux
    wxArrayString paths;
    paths.Add("/usr/local/bin");
    paths.Add("/usr/bin");

    // Add our custom path
    wxFileName path(clStandardPaths::Get().GetUserDataDir(), "");
    path.AppendDir("webtools");
    path.AppendDir("js");
    paths.Add(path.GetPath());

    wxFileName nodejs;
    wxFileName npm;
    if(TryPaths(paths, "nodejs", nodejs) || TryPaths(paths, "node", nodejs) || TryPaths(paths, "node.osx", nodejs)) {
        m_nodejs = nodejs.GetFullPath();
    }

    if(TryPaths(paths, "npm", npm)) {
        m_npm = npm.GetFullPath();
    }

#elif defined(__WXMSW__)
    // Registry paths are searched first
    wxArrayString paths;

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

    // Add CodeLite installed Node.js
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    fn.AppendDir("js");
    paths.Add(fn.GetPath());

    wxFileName nodejs;
    wxFileName npm;
    if(TryPaths(paths, "node.exe", nodejs)) {
        m_nodejs = nodejs.GetFullPath();
    }

    // npm is actually a batch script
    if(TryPaths(paths, "npm.cmd", npm)) {
        m_npm = npm.GetFullPath();
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
