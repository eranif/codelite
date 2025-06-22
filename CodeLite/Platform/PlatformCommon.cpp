#include "PlatformCommon.hpp"

#include "file_logger.h"
#include "procutils.h"

#include <algorithm>
#include <wx/arrstr.h>

std::optional<wxString> PlatformCommon::WhichWithVersion(const wxString& command, const std::vector<int>& versions)
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
        wxString command_fullpath;
        if (Which(name, &command_fullpath)) {
            return command_fullpath;
        }
    }
    return std::nullopt;
}

/// Locate rustup bin folder
/// the path is set to:
/// $HOME/.rustup/TOOLCHAIN-NAME/bin
std::optional<wxString> PlatformCommon::FindRustupToolchainBinDir()
{
#ifdef __WXMSW__
    return std::nullopt;
#else
    const auto homedir = FindHomeDir();

    wxString rustup_exe;
    rustup_exe << homedir.value_or("") << "/.cargo/bin/rustup";
    if (!wxFileName::FileExists(rustup_exe)) {
        return std::nullopt;
    }

    // locate the default toolchain
    wxString toolchain_name = ProcUtils::GrepCommandOutput({ rustup_exe, "toolchain", "list" }, "(default)");
    toolchain_name = toolchain_name.BeforeLast('(');
    toolchain_name.Trim().Trim(false);
    if(toolchain_name.empty()) {
        return std::nullopt;
    }

    // build the path
    wxString rustup_bin_dir;
    rustup_bin_dir << *homedir << "/.rustup/toolchains/" << toolchain_name << "/bin";
    clDEBUG() << "Rust toolchain path:" << rustup_bin_dir << endl;
    return rustup_bin_dir;
#endif
}

std::optional<wxString> PlatformCommon::GetPath(bool useSystemPath)
{
    wxUnusedVar(useSystemPath);
    wxString value;
    if (::wxGetEnv("PATH", &value))
        return value;
    return std::nullopt;
}
