#include "LINUX.hpp"

#include <wx/arrstr.h>
#include <wx/tokenzr.h>

bool LINUX::FindInstallDir(wxString* installpath)
{
    *installpath = "/";
    return true;
}

bool LINUX::FindHomeDir(wxString* homedir)
{
    *homedir << "/home/" << ::wxGetUserId();
    return true;
}

bool LINUX::Which(const wxString& command, wxString* command_fullpath)
{
    wxString pathenv;
    wxGetEnv("PATH", &pathenv);
    wxArrayString paths = ::wxStringTokenize(pathenv, ":", wxTOKEN_STRTOK);
    paths.Insert("/opt/homebrew/bin", 0);
    paths.Insert("/usr/local/bin", 0);
    for(auto path : paths) {
        path << "/" << command;
        if(wxFileName::FileExists(path)) {
            *command_fullpath = path;
            return true;
        }
    }
    return false;
}
