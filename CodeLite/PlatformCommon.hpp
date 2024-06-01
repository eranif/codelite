#ifndef PLAFORM_COMMON_HPP
#define PLAFORM_COMMON_HPP

#include "codelite_exports.h"

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
    PlatformCommon() {}
    virtual ~PlatformCommon() {}

public:
    /**
     * @brief some vendors deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    bool WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath);

    /**
     * @brief locaate rustup bin folder (on platforms that this is available)
     */
    bool FindRustupToolchainBinDir(wxString* rustup_bin_dir);

    /// override this in the platform specific code
    virtual bool Which(const wxString& command, wxString* command_fullpath)
    {
        wxUnusedVar(command);
        wxUnusedVar(command_fullpath);
        return false;
    }

    /// Find the firs command in the array and return its full path
    /// we stop on the first match
    bool AnyWhich(const wxArrayString& commands, wxString* command_fullpath)
    {
        for(const auto& cmd : commands) {
            if(Which(cmd, command_fullpath)) {
                return true;
            }
        }
        return false;
    }

    /// override this in the platform specific code
    /// on macOS, application are usually placed under /Application
    /// are are opened with the `open` command
    virtual bool MacFindApp(const wxString& appname, wxString* command_fullpath, bool new_instance = true)
    {
        wxUnusedVar(appname);
        wxUnusedVar(command_fullpath);
        wxUnusedVar(new_instance);
        return false;
    }

    /**
     * @brief locate msys2 installation folder (e.g. C:/msys2)
     */
    virtual bool FindInstallDir(wxString* msyspath)
    {
        wxUnusedVar(msyspath);
        return false;
    }

    /**
     * @brief locate the home folder within msys2 (e.g. C:/msys2/home/eran)
     */
    virtual bool FindHomeDir(wxString* homedir)
    {
        wxUnusedVar(homedir);
        return false;
    }

    /**
     * @brief set the chroot folder (on linux, this method is a placeholder)
     */
    virtual void SetChroot(const wxString& chroot) { wxUnusedVar(chroot); }

    /**
     * @brief return environment variable value
     */
    virtual bool GetPath(wxString* value, bool useSystemPath = true);
};
#endif // PLAFORM_COMMON_HPP
