#ifndef MSYS2_HPP
#define MSYS2_HPP

#include "codelite_exports.h"

class WXDLLIMPEXP_SDK MSYS2
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
};

#endif // MSYS2_HPP
