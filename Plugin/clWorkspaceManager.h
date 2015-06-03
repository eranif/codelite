#ifndef CLWORKSPACEMANAGER_H
#define CLWORKSPACEMANAGER_H

#include <wx/event.h>
#include "codelite_exports.h"
#include "IWorkspace.h"

class WXDLLIMPEXP_SDK clWorkspaceManager : public wxEvtHandler
{
    IWorkspace* m_workspace;
    IWorkspace::List_t m_workspaces;

protected:
    clWorkspaceManager();
    virtual ~clWorkspaceManager();

public:
    static clWorkspaceManager& Get();

    void SetWorkspace(IWorkspace* workspace) { this->m_workspace = workspace; }
    IWorkspace* GetWorkspace() { return m_workspace; }

    /**
     * @brief register new workspace type
     * @param workspace
     */
    void RegisterWorkspace(IWorkspace* workspace);
};

#endif // CLWORKSPACEMANAGER_H
