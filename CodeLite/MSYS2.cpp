#include "MSYS2.hpp"

namespace
{
bool install_dir_checked = false;
wxString static_install_dir;

bool home_dir_checked = false;
wxString static_home_dir;
} // namespace

bool MSYS2::FindInstallDir(wxString* msyspath)
{
    if(install_dir_checked) {
        *msyspath = static_install_dir;
        return !static_install_dir.empty();
    }

    install_dir_checked = true;

    // try common paths
    std::vector<wxString> vpaths = { R"(C:\msys64)", R"(C:\msys2)", R"(C:\msys)" };
    for(const wxString& path : vpaths) {
        if(wxFileName::DirExists(path)) {
            static_install_dir = path;
            *msyspath = static_install_dir;
            break;
        }
    }
    return !static_install_dir.empty();
}

bool MSYS2::FindHomeDir(wxString* homedir)
{
    wxString msyspath;
    if(!FindInstallDir(&msyspath)) {
        return false;
    }

    if(home_dir_checked) {
        *homedir = static_home_dir;
        return !static_home_dir.empty();
    }

    home_dir_checked = true;

    wxFileName cargo_dir{ msyspath, wxEmptyString };
    cargo_dir.AppendDir("home");
    cargo_dir.AppendDir(::wxGetUserId());

    if(cargo_dir.DirExists()) {
        static_home_dir = cargo_dir.GetPath();
    }

    *homedir = static_home_dir;
    return !static_home_dir.empty();
}

bool MSYS2::Which(const wxString& command, wxString* command_fullpath)
{
    wxString msyspath;
    if(!FindInstallDir(&msyspath)) {
        return false;
    }

    wxArrayString paths_to_try;
    paths_to_try.Add(msyspath + R"(\clang64\bin\)" + command + ".exe");
    paths_to_try.Add(msyspath + R"(\mingw64\bin\)" + command + ".exe");
    paths_to_try.Add(msyspath + R"(\usr\bin\)" + command + ".exe");
    for(const auto& path : paths_to_try) {
        if(wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}
