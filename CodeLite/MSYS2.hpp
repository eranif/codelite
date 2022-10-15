#ifndef MSYS2_HPP
#define MSYS2_HPP

#include "PlatformCommon.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL MSYS2 : public PlatformCommon
{
    bool m_checked_for_install_dir = false;
    wxString m_install_dir;
    bool m_checked_for_home_dir = false;
    wxString m_home_dir;
    wxArrayString m_chroots;

public:
    MSYS2();
    virtual ~MSYS2() {}

    static MSYS2* Get();

public:
    /**
     * @brief locate msys2 installation folder (e.g. C:/msys2)
     */
    bool FindInstallDir(wxString* msyspath) override;
    /**
     * @brief locate the home folder within msys2 (e.g. C:/msys2/home/eran)
     */
    bool FindHomeDir(wxString* homedir) override;

    /**
     * @brief check if a command "command" is installed and return its fullpath
     */
    bool Which(const wxString& command, wxString* command_fullpath) override;

    /**
     * @brief set the chroot folder
     */
    void SetChroot(const wxString& chroot = "\\clang64") override;

    /**
     * @brief some vendros deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    bool WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath);
};

#endif // MSYS2_HPP
