//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clWorkspaceManager.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CLWORKSPACEMANAGER_H
#define CLWORKSPACEMANAGER_H

#include "IWorkspace.h"
#include "clWorkspaceEvent.hpp"
#include "codelite_exports.h"

#include <wx/event.h>

class WXDLLIMPEXP_SDK clWorkspaceManager : public wxEvtHandler
{
    IWorkspace* m_workspace;
    IWorkspace::List_t m_workspaces;

protected:
    clWorkspaceManager();
    virtual ~clWorkspaceManager();

    void OnWorkspaceClosed(clWorkspaceEvent& e);

public:
    static clWorkspaceManager& Get();

    void SetWorkspace(IWorkspace* workspace) { this->m_workspace = workspace; }
    IWorkspace* GetWorkspace() { return m_workspace; }

    /**
     * @brief return list of all supported workspaces
     */
    wxArrayString GetAllWorkspaces() const;

    /**
     * @brief do we have a workspace opened?
     */
    bool IsWorkspaceOpened() const { return m_workspace != NULL; }
    /**
     * @brief register new workspace type
     * @param workspace
     */
    void RegisterWorkspace(IWorkspace* workspace);
};

/// A convenience class with common implementation for local workspace (i.e. non remote)
class WXDLLIMPEXP_SDK IEditor;
class WXDLLIMPEXP_SDK LocalWorkspaceCommon : public IWorkspace
{
public:
    LocalWorkspaceCommon() {}
    virtual ~LocalWorkspaceCommon() {}

    /// Open (create if missing) `filepath` and load it into an editor.
    IEditor* OpenFileInEditor(const wxString& filepath, bool createIfMissing = true) override;
};
#endif // CLWORKSPACEMANAGER_H
