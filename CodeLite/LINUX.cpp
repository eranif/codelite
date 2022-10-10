#include "LINUX.hpp"

#include "PlatformCommon.hpp"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

thread_local wxString RUST_TOOLCHAIN_BIN;
thread_local bool rust_toolchain_scanned = false;

namespace
{
/// Locate rustup bin folder
/// the path is set to:
/// $HOME/.rustup/toolchains/TOOLCHAIN-NAME/bin
bool get_rustup_bin_folder(wxString* rustup_bin_dir)
{
    if(rust_toolchain_scanned) {
        *rustup_bin_dir = RUST_TOOLCHAIN_BIN;
        return !RUST_TOOLCHAIN_BIN.empty();
    }

    PlatformCommon::FindRustupToolchainBinDir(&RUST_TOOLCHAIN_BIN);
    rust_toolchain_scanned = true;

    // call this method again, this time rust_toolchain_scanned is set to true
    return get_rustup_bin_folder(rustup_bin_dir);
}
} // namespace

bool LINUX::FindInstallDir(wxString* installpath)
{
    *installpath = "/";
    return true;
}

bool LINUX::FindHomeDir(wxString* homedir)
{
#ifdef __WXMAC__
    *homedir << "/Users/" << ::wxGetUserId();
#else
    *homedir << "/home/" << ::wxGetUserId();
#endif
    return true;
}

bool LINUX::Which(const wxString& command, wxString* command_fullpath)
{
    wxString HOME;
    FindHomeDir(&HOME);

    wxString pathenv;
    wxGetEnv("PATH", &pathenv);
    wxArrayString paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    paths.Insert("/usr/local/bin", 0);

#ifdef __WXMAC__
    // macOS only
    paths.Insert("/opt/homebrew/bin", 0);
#endif

#ifdef __WXGTK__
    paths.Insert(wxString() << HOME << "/.local/bin", 0);
#endif

    // cargo
    paths.Insert(wxString() << HOME << "/.cargo/bin", 0);

    // rustup
    wxString rustup_bin_folder;
    if(get_rustup_bin_folder(&rustup_bin_folder)) {
        paths.Insert(rustup_bin_folder, 0);
    }

    for(auto path : paths) {
        path << "/" << command;
        if(wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}

bool LINUX::WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath)
{
    return PlatformCommon::WhichWithVersion(command, versions, command_fullpath);
}
