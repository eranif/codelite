#include "GitLocator.h"
#include <wx/tokenzr.h>
#include "globals.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

GitLocator::GitLocator() {}

GitLocator::~GitLocator() {}

bool GitLocator::GetExecutable(wxFileName& gitpath) const
{
    // Common to all platforms:
    // use the PATH environment variable to locate git executable
    wxString path;
    if(::wxGetEnv("PATH", &path)) {
        wxArrayString paths = ::wxStringTokenize(path, ";", wxTOKEN_STRTOK);
        for(size_t i = 0; i < paths.GetCount(); ++i) {
            wxString gitExeFullPath;
            if(DoCheckGitInFolder(paths.Item(i), gitExeFullPath)) {
                gitpath = gitExeFullPath;
                return true;
            }
        }
    }

#ifdef __WXMSW__
    // Under Windows search the registry for git executable as well
    wxRegKey regGit(wxRegKey::HKLM, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1");
    wxRegKey regGit2(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1");
    wxString installLocation;
    if(((regGit.Exists() && regGit.QueryValue("InstallLocation", installLocation)) ||
           (regGit2.Exists() && regGit2.QueryValue("InstallLocation", installLocation))) &&
        ::wxDirExists(installLocation)) {
        wxString gitExeFullPath;
        if(DoCheckGitInFolder(installLocation, gitExeFullPath)) {
            gitpath = gitExeFullPath;
            return true;
        }
    }
#endif
    return false;
}

bool GitLocator::DoCheckGitInFolder(const wxString& folder, wxString& git) const
{
    wxFileName gitExe(folder, "git.exe");
    if(gitExe.Exists()) {
        git = gitExe.GetFullPath();
        return true;
    }

    // try to see if we have git.exe under an internal folder
    gitExe.AppendDir("bin");
    if(gitExe.Exists()) {
        git = gitExe.GetFullPath();
        return true;
    }
    return false;
}

bool GitLocator::MSWGetGitShellCommand(wxString& bashCommand) const
{
#ifdef __WXMSW__
    wxRegKey regGit(wxRegKey::HKLM, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1");
    wxRegKey regGit2(wxRegKey::HKLM, "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Git_is1");
    wxString installLocation;
    if(((regGit.Exists() && regGit.QueryValue("InstallLocation", installLocation)) ||
           (regGit2.Exists() && regGit2.QueryValue("InstallLocation", installLocation))) &&
        ::wxDirExists(installLocation)) {
        wxString gitExeFullPath;
        if(DoCheckGitInFolder(installLocation, gitExeFullPath)) {
            wxFileName gitpath = gitExeFullPath;
            gitpath.SetName("bash");
            if(gitpath.Exists()) {
                bashCommand = gitpath.GetFullPath();
                ::WrapWithQuotes(bashCommand);
                bashCommand << " --login -i";
                return true;
            }
        }
    }
#endif
    return false;
}
