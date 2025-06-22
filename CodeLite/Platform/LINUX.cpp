#include "LINUX.hpp"

#include "PlatformCommon.hpp"
#include "StringUtils.h"
#include "clFilesCollector.h"
#include "clVersionString.hpp"
#include "file_logger.h"
#include "procutils.h"

#include <algorithm>
#include <cmath>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/utils.h>

namespace
{
#ifdef __WXMAC__
/// Homebrew install formulas in a specific location, this function
/// attempts to discover this location
bool macos_find_homebrew_opt_formula_dir(const wxString& formula, wxString* install_path)
{
    wxFileName fn("/opt/homebrew/opt", wxEmptyString);
    fn.AppendDir(formula);
    fn.AppendDir("bin");
    if (fn.DirExists()) {
        *install_path = fn.GetPath();
        return true;
    }
    return false;
}
#endif
} // namespace

/// Locate rustup bin folder
/// the path is set to:
/// $HOME/.rustup/toolchains/TOOLCHAIN-NAME/bin
std::optional<wxString> LINUX::get_rustup_bin_folder()
{
    if (!rust_toolchain_scanned) {
        RUST_TOOLCHAIN_BIN = FindRustupToolchainBinDir();
        rust_toolchain_scanned = true;
    }
    return RUST_TOOLCHAIN_BIN;
}

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
    wxString pathenv;
    GetPath(&pathenv, m_flags & SEARCH_PATH_ENV);
    wxArrayString paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    for (auto path : paths) {
        path << "/" << command;
        if (wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}

bool LINUX::GetPath(wxString* value, bool useSystemPath)
{
    wxString HOME;
    FindHomeDir(&HOME);

    wxArrayString special_paths;

#ifdef __WXMAC__
    // macOS only
    special_paths.Add("/opt/homebrew/bin");
#endif

#if defined(__WXGTK__) || defined(__WXMAC__)
    // both macOS and Linux are using this path
    special_paths.Add(wxString() << HOME << "/.local/bin");
#if defined(__WXGTK__)
    // linux also supports homebrew
    if (wxFileName::DirExists("/home/linuxbrew/.linuxbrew/bin")) {
        special_paths.Add("/home/linuxbrew/.linuxbrew/bin");
    }
#endif
#endif

    // cargo
    special_paths.Add(wxString() << HOME << "/.cargo/bin");

    // rustup
    if (const auto rustup_bin_folder = get_rustup_bin_folder()) {
        special_paths.Add(*rustup_bin_folder);
    }

    // /usr/local/bin is not always in the PATH, so add it
    special_paths.Add("/usr/local/bin");

    // common paths: read the env PATH and append the other paths to it
    // so common paths found ENV:PATH will come first
    wxArrayString paths;
    if (useSystemPath) {
        wxString pathenv;
        ::wxGetEnv("PATH", &pathenv);
        paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    }

    // append the special paths to the end
    paths.insert(paths.end(), special_paths.begin(), special_paths.end());

#ifdef __WXMAC__
    // llvm is placed under a special location
    // if we find it, we place it first
    wxString llvm_path;
    if (macos_find_homebrew_opt_formula_dir("llvm", &llvm_path)) {
        paths.Insert(llvm_path, 0);
    }
#endif

    // remove duplicate entries
    wxStringSet_t S;
    wxArrayString unique_paths;
    unique_paths.reserve(paths.size());
    for (const auto& path : paths) {
        if (S.count(path) == 0) {
            S.insert(path);
            unique_paths.Add(path);
        }
    }

    paths.swap(unique_paths);
    *value = wxJoin(paths, ':');
    return true;
}

std::optional<wxString> LINUX::WhichWithVersion(const wxString& command, const std::vector<int>& versions)
{
    return PlatformCommon::WhichWithVersion(command, versions);
}

namespace
{
thread_local LINUX instance;
}
LINUX* LINUX::Get() { return &instance; }

bool LINUX::MacFindApp(const wxString& appname, wxString* command_fullpath, bool new_instance)
{
#ifdef __WXMAC__
    wxFileName path{ "/Applications", wxEmptyString };
    path.AppendDir(appname + ".app");

    // search for app name
    if (path.DirExists()) {
        (*command_fullpath) << "/usr/bin/open ";
        if (new_instance) {
            (*command_fullpath) << "-n ";
        }
        (*command_fullpath) << StringUtils::WrapWithDoubleQuotes(path.GetPath());
        return true;
    }
#endif
    return false;
}