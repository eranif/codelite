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

std::optional<wxString> LINUX::FindInstallDir()
{
    return "/";
}

std::optional<wxString> LINUX::FindHomeDir()
{
#ifdef __WXMAC__
    return wxString() << "/Users/" << ::wxGetUserId();
#else
    return wxString() << "/home/" << ::wxGetUserId();
#endif
}

std::optional<wxString> LINUX::Which(const wxString& command)
{
    const wxString pathenv = GetPath(m_flags & SEARCH_PATH_ENV).value_or("");
    wxArrayString paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    for (auto path : paths) {
        path << "/" << command;
        if (wxFileName::FileExists(path)) {
            return path;
        }
    }
    return std::nullopt;
}

std::optional<wxString> LINUX::GetPath(bool useSystemPath)
{
    const auto HOME = FindHomeDir();

    wxArrayString special_paths;

#ifdef __WXMAC__
    // macOS only
    special_paths.Add("/opt/homebrew/bin");
#endif

#if defined(__WXGTK__) || defined(__WXMAC__)
    // both macOS and Linux are using this path
    special_paths.Add(wxString() << HOME.value_or("") << "/.local/bin");
#if defined(__WXGTK__)
    // linux also supports homebrew
    if (wxFileName::DirExists("/home/linuxbrew/.linuxbrew/bin")) {
        special_paths.Add("/home/linuxbrew/.linuxbrew/bin");
    }
#endif
#endif

    // cargo
    special_paths.Add(wxString() << HOME.value_or("") << "/.cargo/bin");

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
    return wxJoin(paths, ':');
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

std::optional<wxString> LINUX::MacFindApp(const wxString& appname, bool new_instance)
{
#ifdef __WXMAC__
    wxFileName path{ "/Applications", wxEmptyString };
    path.AppendDir(appname + ".app");

    // search for app name
    if (path.DirExists()) {
        wxString command_fullpath;
        command_fullpath << "/usr/bin/open ";
        if (new_instance) {
            command_fullpath << "-n ";
        }
        command_fullpath << StringUtils::WrapWithDoubleQuotes(path.GetPath());
        return command_fullpath;
    }
#endif
    return std::nullopt;
}
