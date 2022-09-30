#ifndef MSYS2_HPP
#define MSYS2_HPP

#include "codelite_exports.h"

class WXDLLIMPEXP_CL MSYS2
{
public:
    /**
     * @brief locate msys2 installation folder (e.g. C:/msys2)
     */
    static bool FindInstallDir(wxString* msyspath);
    /**
     * @brief locate the home folder within msys2 (e.g. C:/msys2/home/eran)
     */
    static bool FindHomeDir(wxString* homedir);

    /**
     * @brief check if a command "command" is installed and return its fullpath
     */
    static bool Which(const wxString& command, wxString* command_fullpath);
};

#endif // MSYS2_HPP
