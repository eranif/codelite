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

/// Homebrew install formulas in a specific location, this function
/// attempts to discover this location
bool macos_find_homebrew_cellar_path_for_formula(const wxString& formula, wxString* install_path)
{
    wxString cellar_path = "/opt/homebrew/Cellar";
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

#ifdef __WXMAC__
    // llvm is placed under a special location
    wxString llvm_path;
    if(macos_find_homebrew_cellar_path_for_formula("llvm", &llvm_path)) {
        paths.Insert(llvm_path + "/bin", 0);
    }
#endif

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
