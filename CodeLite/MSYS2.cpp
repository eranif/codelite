#include "MSYS2.hpp"

#include "cl_standard_paths.h"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

bool MSYS2::FindInstallDir(wxString* msyspath)
{
    if(m_checked_for_install_dir) {
        *msyspath = m_install_dir;
        return !m_install_dir.empty();
    }

    m_checked_for_install_dir = true;

    // try common paths
    std::vector<wxString> vpaths = { R"(C:\msys64)", R"(C:\msys2)", R"(C:\msys)" };
    for(const wxString& path : vpaths) {
        if(wxFileName::DirExists(path)) {
            m_install_dir = path;
            *msyspath = m_install_dir;
            break;
        }
    }
    return !m_install_dir.empty();
}

bool MSYS2::FindHomeDir(wxString* homedir)
{
    wxString msyspath;
    if(!FindInstallDir(&msyspath)) {
        return false;
    }

    if(m_checked_for_home_dir) {
        *homedir = m_home_dir;
        return !m_home_dir.empty();
    }

    m_checked_for_home_dir = true;

    wxFileName cargo_dir{ msyspath, wxEmptyString };
    cargo_dir.AppendDir("home");
    cargo_dir.AppendDir(::wxGetUserId());

    if(cargo_dir.DirExists()) {
        m_home_dir = cargo_dir.GetPath();
    }

    *homedir = m_home_dir;
    return !m_home_dir.empty();
}

bool MSYS2::Which(const wxString& command, wxString* command_fullpath)
{
    wxString msyspath;
    bool has_msys2 = FindInstallDir(&msyspath);

    wxArrayString paths_to_try;

    // next in order are is the PATH environment variable
    if(m_flags & SEARCH_PATH_ENV) {
        wxString pathenv;
        wxGetEnv("PATH", &pathenv);
        paths_to_try = ::wxStringTokenize(pathenv, ";", wxTOKEN_STRTOK);
    }

    // add the executable path (this is how windows work: we first look at the executable path)
    paths_to_try.Insert(wxFileName(clStandardPaths::Get().GetExecutablePath()).GetPath(), 0);

    // if we have msys2 installed, add the bin folder (we place them at start)
    if(has_msys2) {
        for(const auto& root : m_chroots) {
            paths_to_try.Insert(msyspath + root + R"(\bin)", 0);
        }
    }

    // at the point, the order of search is:
    // MSYS2 -> Executable path -> PATH paths
    for(auto path : paths_to_try) {
        path << "\\" << command << ".exe";
        if(wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}

bool MSYS2::WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath)
{
    return PlatformCommon::WhichWithVersion(command, versions, command_fullpath);
}

void MSYS2::SetChroot(const wxString& chroot)
{
    m_chroots.clear();
    m_chroots.Add(chroot);

    // exclude PATH from the search path
    m_flags &= ~SEARCH_PATH_ENV;
}

namespace
{
thread_local MSYS2 instance;
}

MSYS2* MSYS2::Get() { return &instance; }

MSYS2::MSYS2()
{
    // last entry -> most important (reverse order)
    m_chroots.Add(R"(\usr)");
    m_chroots.Add(R"(\mingw64)");
    m_chroots.Add(R"(\clang64)");
}
