#include "LINUX.hpp"

#include "PlatformCommon.hpp"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

bool LINUX::FindInstallDir(wxString* installpath)
{
    *installpath = "/";
    return true;
}

bool LINUX::FindHomeDir(wxString* homedir)
{
#ifdef __WXMAC__
    *homedir << "/Users/" << ::wxGetUserId();
#else
    *homedir << "/home/" << ::wxGetUserId();
#endif
    return true;
}

bool LINUX::Which(const wxString& command, wxString* command_fullpath)
{
    wxString pathenv;
    wxGetEnv("PATH", &pathenv);
    wxArrayString paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    paths.Insert("/opt/homebrew/bin", 0);
    paths.Insert("/usr/local/bin", 0);
    wxString homedir;
    if(FindHomeDir(&homedir)) {
        homedir << "/.cargo/bin";
        paths.Insert(homedir, 0);
    }

    for(auto path : paths) {
        path << "/" << command;
        if(wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}

bool LINUX::WhichWithVersion(const wxString& command, const std::vector<int>& versions, wxString* command_fullpath)
{
    return PlatformCommon::WhichWithVersion(command, versions, command_fullpath);
}
