#ifndef CLFILESYSTEMWORKSPACE_HPP
#define CLFILESYSTEMWORKSPACE_HPP

#include "codelite_exports.h"
#include "IWorkspace.h"
#include <wx/arrstr.h>
#include <vector>
#include "cl_command_event.h"
#include <unordered_map>

class clFileSystemWorkspaceView;
class WXDLLIMPEXP_SDK clFileSystemWorkspace : public IWorkspace
{
    std::vector<wxString> m_files;
    wxFileName m_filename;
    bool m_isLoaded = false;
    bool m_showWelcomePage = false;
    bool m_dummy = true;
    
    // Workspace settings
    size_t m_flags = 0;
    std::unordered_map<wxString, wxString> m_buildTargets;
    clFileSystemWorkspaceView* m_view = nullptr;

protected:
    void CacheFiles();

    //===--------------------------
    // Event handlers
    //===--------------------------
    void OnBuildStarting(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);

    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void RestoreSession();

protected:
    bool Load(const wxFileName& file);
    void Save();
    void DoOpen();
    void DoClose();
    void DoClear();

public:
    ///===--------------------------
    /// IWorkspace interface
    ///===--------------------------
    virtual wxString GetActiveProjectName() const;
    virtual wxFileName GetFileName() const;
    virtual wxString GetFilesMask() const;
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;

    clFileSystemWorkspace(bool dummy);
    virtual ~clFileSystemWorkspace();
    
    static clFileSystemWorkspace& Get();
    
    ///===--------------------------
    /// Specific API
    ///===--------------------------
    clFileSystemWorkspaceView* GetView() { return m_view; }
    
    /**
     * @brief create an empty workspace at a given folder
     * @param folder
     */
    void New(const wxString& folder);
};

#endif // CLFILESYSTEMWORKSPACE_HPP
