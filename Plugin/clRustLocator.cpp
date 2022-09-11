#include "clRustLocator.hpp"

#include "MSYS2.hpp"
#include "fileutils.h"

#include <wx/arrstr.h>
#include <wx/filename.h>

clRustLocator::clRustLocator() {}

clRustLocator::~clRustLocator() {}

bool clRustLocator::Locate()
{
    wxString homedir;
    wxString toolname = "cargo";
#ifdef __WXMSW__
    // try common paths
    wxString msyspath;
    if(!MSYS2::FindInstallDir(&msyspath)) {
        return false;
    }

    std::vector<wxString> vpaths = { msyspath + "\\clang64\\bin", msyspath + "\\mingw64\\bin" };

    wxArrayString paths;
    paths.reserve(vpaths.size());
    for(const auto& path : vpaths) {
        paths.Add(path);
    }

    // locate cargo.exe
    wxFileName tool_exe;
    if(!FileUtils::FindExe(toolname, tool_exe, paths)) {
        return false;
    }

    m_binDir = tool_exe.GetPath();
    return true;
#else
    // try common paths
    ::wxGetEnv("HOME", &homedir);
    wxFileName cargo_dir(homedir, wxEmptyString);
    cargo_dir.AppendDir(".cargo");
    wxArrayString paths;
    paths.Add(cargo_dir.GetPath()); // manual installation goes here
    paths.Add("/usr/bin");          // package managers will put things here

    wxFileName tool_exe;
    if(!FileUtils::FindExe(toolname, tool_exe, paths)) {
        return false;
    }
    m_binDir = tool_exe.GetPath();
    return true;
#endif
}

wxString clRustLocator::GetRustTool(const wxString& name) const
{
    if(m_binDir.empty()) {
        return wxEmptyString;
    }

    wxFileName toolpath(m_binDir, name);
#ifdef __WXMSW__
    toolpath.SetExt("exe");
#endif
    if(toolpath.FileExists()) {
        return toolpath.GetFullPath();
    }
    return wxEmptyString;
}
