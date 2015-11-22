#include "NodeJSLocator.h"
#include "cl_standard_paths.h"

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
    // Windows
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    fn.AppendDir("js");
    
    // TODO: add path based on the registry entries
    wxArrayString paths;
    paths.Add(fn.GetPath());

    wxFileName nodejs;
    if(TryPaths(paths, "node.exe", nodejs)) {
        m_nodejs = nodejs.GetFullPath();
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
