#ifndef PLAFORM_COMMON_HPP
#define PLAFORM_COMMON_HPP

#include "codelite_exports.h"

#include <optional>
#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

class WXDLLIMPEXP_CL PlatformCommon
{
public:
    enum Flags {
        SEARCH_PATH_ENV = (1 << 0),
    };

protected:
    size_t m_flags = SEARCH_PATH_ENV;

public:
    PlatformCommon() = default;
    virtual ~PlatformCommon() = default;

public:
    /**
     * @brief some vendors deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    std::optional<wxString> WhichWithVersion(const wxString& command, const std::vector<int>& versions);

    /**
     * @brief locate rustup bin folder (on platforms that this is available)
     */
    std::optional<wxString> FindRustupToolchainBinDir();

    /// override this in the platform specific code
    virtual std::optional<wxString> Which(const wxString& command)
    {
        wxUnusedVar(command);
        return std::nullopt;
    }

    /// Find the first command in the array and return its full path
    /// we stop on the first match
    std::optional<wxString> AnyWhich(const wxArrayString& commands)
    {
        for (const auto& cmd : commands) {
            if (const auto command_fullpath = Which(cmd)) {
                return command_fullpath;
            }
        }
        return std::nullopt;
    }

    /// override this in the platform specific code
    /// on macOS, applications are usually placed under /Application
    /// and are opened with the `open` command
    virtual std::optional<wxString> MacFindApp(const wxString& appname, bool new_instance = true)
    {
        wxUnusedVar(appname);
        wxUnusedVar(new_instance);
        return std::nullopt;
    }

    /**
     * @brief locate msys2 installation folder (e.g. C:/msys2)
     */
    virtual std::optional<wxString> FindInstallDir() { return std::nullopt; }

    /**
     * @brief locate the home folder within msys2 (e.g. C:/msys2/home/eran)
     */
    virtual std::optional<wxString> FindHomeDir() { return std::nullopt; }

    /**
     * @brief set the chroot folder (on linux, this method is a placeholder)
     */
    virtual void SetChroot(const wxString& chroot) { wxUnusedVar(chroot); }

    /**
     * @brief return environment variable value
     */
    virtual std::optional<wxString> GetPath(bool useSystemPath = true);
};
#endif // PLAFORM_COMMON_HPP
