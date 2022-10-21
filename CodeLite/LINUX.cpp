#include "LINUX.hpp"

#include "PlatformCommon.hpp"
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
bool macos_find_homebrew_cellar_path_for_formula(const wxString& formula, wxString* install_path)
{
    wxString cellar_path = "/opt/homebrew/Cellar";
    if (!wxFileName::DirExists(cellar_path)) {
        cellar_path = "/usr/local/Cellar";
    }
    
    cellar_path << "/" << formula;

    if(!wxFileName::DirExists(cellar_path)) {
        return false;
    }

    // we take the string with the highest value
    clFilesScanner fs;
    clFilesScanner::EntryData::Vec_t results;
    if(fs.ScanNoRecurse(cellar_path, results) == 0) {
        return false;
    }

    // we are only interested in the name part
    for(auto& result : results) {
        result.fullpath = result.fullpath.AfterLast('/');
    }

    std::sort(results.begin(), results.end(),
              [](const clFilesScanner::EntryData& a, const clFilesScanner::EntryData& b) {
                  clVersionString vs_a{ a.fullpath };
                  clVersionString vs_b{ b.fullpath };
                  // want to sort in descending order
                  return vs_b.to_number() < vs_a.to_number();
              });

    *install_path << cellar_path << "/" << results[0].fullpath;
    clDEBUG() << "Using cellar path:" << *install_path << endl;
    return true;
}
#endif
} // namespace

/// Locate rustup bin folder
/// the path is set to:
/// $HOME/.rustup/toolchains/TOOLCHAIN-NAME/bin
bool LINUX::get_rustup_bin_folder(wxString* rustup_bin_dir)
{
    if(rust_toolchain_scanned) {
        *rustup_bin_dir = RUST_TOOLCHAIN_BIN;
        return !RUST_TOOLCHAIN_BIN.empty();
    }

    FindRustupToolchainBinDir(&RUST_TOOLCHAIN_BIN);
    rust_toolchain_scanned = true;

    // call this method again, this time rust_toolchain_scanned is set to true
    return get_rustup_bin_folder(rustup_bin_dir);
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
    wxString HOME;
    FindHomeDir(&HOME);

    wxArrayString special_paths;

#ifdef __WXMAC__
    // macOS only
    special_paths.Add("/opt/homebrew/bin");
#endif

#ifdef __WXGTK__
    special_paths.Add(wxString() << HOME << "/.local/bin");
    // linux also supports homebrew
    if(wxFileName::DirExists("/home/linuxbrew/.linuxbrew/bin")) {
        special_paths.Add("/home/linuxbrew/.linuxbrew/bin");
    }
#endif

    // cargo
    special_paths.Add(wxString() << HOME << "/.cargo/bin");

    // rustup
    wxString rustup_bin_folder;
    if(get_rustup_bin_folder(&rustup_bin_folder)) {
        special_paths.Add(rustup_bin_folder);
    }

    // /usr/local/bin is not always in the PATH, so add it
    special_paths.Add("/usr/local/bin");

    // common paths: read the env PATH and append the other paths to it
    // so common paths found ENV:PATH will come first
    wxArrayString paths;
    if(m_flags & SEARCH_PATH_ENV) {
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
    if(macos_find_homebrew_cellar_path_for_formula("llvm", &llvm_path)) {
        paths.Insert(llvm_path + "/bin", 0);
    }
#endif

    // remove duplicate entries
    wxStringSet_t S;
    wxArrayString unique_paths;
    unique_paths.reserve(paths.size());
    for(const auto& path : paths) {
        if(S.count(path) == 0) {
            S.insert(path);
            unique_paths.Add(path);
        }
    }

    paths.swap(unique_paths);

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

namespace
{
thread_local LINUX instance;
}
LINUX* LINUX::Get() { return &instance; }
