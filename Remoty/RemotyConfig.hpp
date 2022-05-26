#ifndef REMOTYCONFIG_HPP
#define REMOTYCONFIG_HPP

#include <vector>
#include <wx/arrstr.h>
#include <wx/string.h>

struct RemoteWorkspaceInfo {
    wxString account;
    wxString path;
};

class RemotyConfig
{
public:
    RemotyConfig();
    ~RemotyConfig();

    /**
     * @brief read the recent workspaces from the file system
     * @return
     */
    std::vector<RemoteWorkspaceInfo> GetRecentWorkspaces() const;

    /**
     * @brief update recent workspaces with workspace
     * placing it at the top ("last used")
     * @param workspace
     */
    void UpdateRecentWorkspaces(const RemoteWorkspaceInfo& workspaceInfo);
    /**
     * @brief return true if the "Open Workspace" dialog should start with "Local" or "Remote"
     */
    bool IsOpenWorkspaceTypeLocal() const;

    /**
     * @brief store the last preference entered by the user in the "Open Workspace" dialog
     */
    void SetOpenWorkspaceTypeLocal(bool local);
};

#endif // REMOTYCONFIG_HPP
