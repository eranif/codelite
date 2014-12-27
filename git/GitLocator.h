#ifndef GITLOCATOR_H
#define GITLOCATOR_H

#include <wx/filename.h>

class GitLocator
{
    bool DoCheckGitInFolder(const wxString &folder, wxString& git) const;
public:
    GitLocator();
    virtual ~GitLocator();
    
    bool GetExecutable(wxFileName& gitpath) const;
    /**
     * @brief return the command required to open a git shell
     */
    bool MSWGetGitShellCommand(wxString& bashCommand) const;
};

#endif // GITLOCATOR_H
