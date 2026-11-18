#pragma once

#include "IWorkspace.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK WorkspaceHelper
{
public:
    WorkspaceHelper() = default;
    ~WorkspaceHelper() = default;

    /// return the full path of a workspace private file (`WORKSPACE_DIR/.codelite/<filename>`).
    /// For a remote workspace, the path is a remote path (with `/` separators)
    wxString GetPrivateFilePath(const IWorkspace* workspace, const wxString& filename) const;

    /// read a workspace private file. "private" files are files kept under `WORKSPACE_PATH/.codelite` folder
    /// this method handles both remote and local workspaces
    bool ReadPrivateFile(IWorkspace* workspace, const wxString& filename, wxString* content) const;

    /// Store a workspace private file
    /// this method handles both remote and local workspaces
    bool WritePrivateFile(IWorkspace* workspace, const wxString& filename, const wxString& content) const;
};
