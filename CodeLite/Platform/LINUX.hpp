#ifndef LINUX_HPP
#define LINUX_HPP

#include "PlatformCommon.hpp"
#include "codelite_exports.h"

#include <vector>
#include <wx/string.h>

class WXDLLIMPEXP_CL LINUX : public PlatformCommon
{
    wxString RUST_TOOLCHAIN_BIN;
    bool rust_toolchain_scanned = false;

protected:
    bool get_rustup_bin_folder(wxString* rustup_bin_dir);

public:
    LINUX() {}
    virtual ~LINUX() {}

    static LINUX* Get();

public:
    /**
     * @brief on linux it is simple "/"
     */
    bool FindInstallDir(wxString* installpath) override;

    /**
     * @brief locate the home folder within msys2 (e.g. /home/eran)
     */
    bool FindHomeDir(wxString* homedir) override;

    /**
     * @brief check if a command "command" is installed and return its fullpath
     */
    bool Which(const wxString& command, wxString* command_fullpath) override;

    /// on macOS, application are usually placed under /Application
    /// are are opened with the `open` command
    bool MacFindApp(const wxString& appname, wxString* command_fullpath, bool new_instance = true) override;

    /**
     * @brief some vendors deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    bool WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath);

    /**
     * @brief return environment variable value
     */
    bool GetPath(wxString* value, bool useSystemPath = true) override;
};

#endif // LINUX_HPP
