#ifndef PLAFORM_COMMON_HPP
#define PLAFORM_COMMON_HPP

#include "codelite_exports.h"

#include <vector>
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
     * @brief some vendros deliver binaries with "-N" where N is the version number
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
};
#endif // PLAFORM_COMMON_HPP
