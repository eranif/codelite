#include "PlatformCommon.hpp"

#include "file_logger.h"
#include "procutils.h"

#include <algorithm>
#include <wx/arrstr.h>

bool PlatformCommon::WhichWithVersion(const wxString& command, const std::vector<int>& versions,
                                      wxString* command_fullpath)
{
    std::vector<int> sorted_versions = versions;

    // sort the numbers in descending order
    std::sort(sorted_versions.begin(), sorted_versions.end(), [](int a, int b) { return b < a; });

    wxArrayString names;
    names.reserve(sorted_versions.size() + 1);

    // executable without number should come first
    names.Add(command);
    for(auto ver : sorted_versions) {
        names.Add(wxString() << command << "-" << ver);
    }
    for(const wxString& name : names) {
        if(Which(name, command_fullpath)) {
            return true;
        }
    }
    return false;
}

/// Locate rustup bin folder
/// the path is set to:
/// $HOME/.rustup/TOOLCHAIN-NAME/bin
bool PlatformCommon::FindRustupToolchainBinDir(wxString* rustup_bin_dir)
{
#ifdef __WXMSW__
    return false;
#else
    wxString homedir;
    FindHomeDir(&homedir);

    wxString rustup_exe;
    rustup_exe << homedir << "/.cargo/bin/rustup";
    if(!wxFileName::FileExists(rustup_exe)) {
        return false;
    }

    // locate the default toolchain
    wxString toolchain_name = ProcUtils::GrepCommandOutput({ rustup_exe, "toolchain", "list" }, "(default)");
    toolchain_name = toolchain_name.BeforeLast('(');
    toolchain_name.Trim().Trim(false);
    if(toolchain_name.empty()) {
        return false;
    }

    // build the path
    *rustup_bin_dir << homedir << "/.rustup/toolchains/" << toolchain_name << "/bin";
    clDEBUG() << "Rust toolchain path:" << *rustup_bin_dir << endl;
    return true;
#endif
}
