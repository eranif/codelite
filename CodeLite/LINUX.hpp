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
};

#endif // LINUX_HPP
