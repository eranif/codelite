#ifndef WORKSPACEHELPER_HPP
#define WORKSPACEHELPER_HPP

#include "IWorkspace.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK WorkspaceHelper
{
public:
    WorkspaceHelper();
    ~WorkspaceHelper();

    /// read a workspace private file. "private" files are files kept under `WORKSPACE_PATH/.codelite` folder
    /// this method handles both remote and local workspaces
    bool ReadPrivateFile(IWorkspace* workspace, const wxString& filename, wxString* content) const;

    /// Store a workspace private file
    /// this method handles both remote and local workspaces
    bool WritePrivateFile(IWorkspace* workspace, const wxString& filename, const wxString& content) const;
};

#endif // WORKSPACEHELPER_HPP
