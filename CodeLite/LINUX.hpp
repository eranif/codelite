#ifndef LINUX_HPP
#define LINUX_HPP

class LINUX
{
public:
    /**
     * @brief on linux it is simple "/"
     */
    static bool FindInstallDir(wxString* installpath);

    /**
     * @brief locate the home folder within msys2 (e.g. /home/eran)
     */
    static bool FindHomeDir(wxString* homedir);

    /**
     * @brief check if a command "command" is installed and return its fullpath
     */
    static bool Which(const wxString& command, wxString* command_fullpath);
    /**
     * @brief some vendros deliver binaries with "-N" where N is the version number
     * this method attempts to search for command-<N>...command (in this order)
     */
    static bool WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath);
};

#endif // LINUX_HPP
