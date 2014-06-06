#include "cl_standard_paths.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>

clStandardPaths::clStandardPaths()
{
}

clStandardPaths::~clStandardPaths()
{
}

clStandardPaths& clStandardPaths::Get()
{
    static clStandardPaths codelitePaths;
    return codelitePaths;
}

wxString clStandardPaths::GetUserDataDir() const
{
    // If the user has provided an alternative datadir, use it
    if (!m_path.empty()) {
        return m_path;
    }

#ifdef __WXGTK__

#ifndef NDEBUG

    // Debug mode
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir());
    fn.SetFullName( fn.GetFullName() + "-dbg" );
    return fn.GetFullPath();

#else
    // Release mode
    return wxStandardPaths::Get().GetUserDataDir();
    
#endif
#else // Windows / OSX
    return wxStandardPaths::Get().GetUserDataDir();
#endif
}

void clStandardPaths::SetUserDataDir(const wxString& path)
{
    m_path = path;
}

wxString clStandardPaths::GetPluginsDirectory() const
{
#ifdef __WXGTK__
    wxString pluginsDir = PLUGINS_DIR;
#else
    wxString pluginsDir = GetDataDir();
#endif
    
    return pluginsDir;
}

wxString clStandardPaths::GetDataDir() const
{
    return wxStandardPaths::Get().GetDataDir();
}
