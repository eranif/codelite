#ifndef NOTEJSWORKSPACE_H
#define NOTEJSWORKSPACE_H

#include "IWorkspace.h"
#include <wx/filename.h>
#include "cl_command_event.h"

class NodeJSWorkspaceView;
class NodeJSWorkspace : public IWorkspace
{
protected:
    wxFileName m_filename;
    wxArrayString m_folders;
    NodeJSWorkspaceView* m_view;
    bool m_clangOldFlag;
    bool m_dummy;
    
    static NodeJSWorkspace* ms_workspace;

protected:
    void DoClear();
    void DoOpen(const wxFileName& filename);

    // Event handlers
    /**
     * @brief CodeLite requests to close the workspace
     */
    void OnCloseWorkspace(clCommandEvent& e);
    /**
     * @brief create new workspace (originated from the menu bar)
     */
    void OnNewWorkspace(wxCommandEvent& e);
    
    NodeJSWorkspace(); // default ctor is private
    
public:
    NodeJSWorkspace(bool dummy);
    virtual ~NodeJSWorkspace();

    static NodeJSWorkspace* Get();
    static void Free();

public:
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

    NodeJSWorkspaceView* GetView() { return m_view; }

    /**
     * @brief is this workspace opened?
     */
    bool IsOpen() const;

    /**
     * @brief create nodejs at the given path
     */
    bool Create(const wxFileName& filename);

    /**
     * @brief open nodejs workspace and load its view
     */
    bool Open(const wxFileName& filename);

    /**
     * @brief close the current workspace
     * this function fires the wxEVT_WORKSPACE_CLOSED event
     */
    void Close();

    /**
     * @brief save the workspace content to the file system
     */
    void Save();

    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    const wxFileName& GetFilename() const { return m_filename; }

    void SetFolders(const wxArrayString& folders) { this->m_folders = folders; }
    const wxArrayString& GetFolders() const { return m_folders; }
};

#endif // NOTEJSWORKSPACE_H
