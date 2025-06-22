#ifndef LINUX_HPP
#define LINUX_HPP

#include "PlatformCommon.hpp"
#include "codelite_exports.h"

#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL LINUX : public PlatformCommon
{
    std::optional<wxString> RUST_TOOLCHAIN_BIN;
    bool rust_toolchain_scanned = false;

protected:
    std::optional<wxString> get_rustup_bin_folder();

public:
    LINUX() {}
    virtual ~LINUX() {}

    static LINUX* Get();

public:
    /**
     * @brief on linux it is simple "/"
     */
    std::optional<wxString> FindInstallDir() override;

    /**
     * @brief locate the home folder within msys2 (e.g. /home/eran)
     */
    std::optional<wxString> FindHomeDir() override;

    /**
     * @brief check if a command "command" is installed and return its fullpath
     */
    bool Which(const wxString& command, wxString* command_fullpath) override;

    /// on macOS, applications are usually placed under /Application
    /// and are opened with the `open` command
    std::optional<wxString> MacFindApp(const wxString& appname, bool new_instance = true) override;

    /**
     * @brief some vendors deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    std::optional<wxString> WhichWithVersion(const wxString& command, const std::vector<int>& versions);

    /**
     * @brief return environment variable value
     */
    std::optional<wxString> GetPath(bool useSystemPath = true) override;
};

#endif // LINUX_HPP
