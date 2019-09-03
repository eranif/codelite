#ifndef CLFILESYSTEMWORKSPACE_HPP
#define CLFILESYSTEMWORKSPACE_HPP

#include "codelite_exports.h"
#include "IWorkspace.h"
#include <wx/arrstr.h>
#include <vector>
#include "cl_command_event.h"
#include <unordered_map>
#include "clFileSystemEvent.h"
#include "macros.h"
#include "asyncprocess.h"

class clFileSystemWorkspaceView;
class WXDLLIMPEXP_SDK clFileSystemWorkspace : public IWorkspace
{
    std::vector<wxFileName> m_files;
    wxFileName m_filename;
    bool m_isLoaded = false;
    bool m_showWelcomePage = false;
    bool m_dummy = true;
    wxArrayString m_compileFlags;
    wxString m_fileExtensions;
    bool m_fileScanNeeded = false;
    IProcess* m_buildProcess = nullptr;

    // Workspace settings
    size_t m_flags = 0;
    wxStringMap_t m_buildTargets;
    clFileSystemWorkspaceView* m_view = nullptr;

protected:
    void CacheFiles();
    wxString CompileFlagsAsString(const wxArrayString& arr) const;
    wxString GetTargetCommand(const wxString& target) const;
    void DoPrintBuildMessage(const wxString& message);
    
    //===--------------------------
    // Event handlers
    //===--------------------------
    void OnBuildStarting(clBuildEvent& event);
    void OnBuildEnded(clBuildEvent& event);

    void OnOpenWorkspace(clCommandEvent& event);
    void OnCloseWorkspace(clCommandEvent& event);
    void OnAllEditorsClosed(wxCommandEvent& event);
    void OnScanCompleted(clFileSystemEvent& event);
    void OnParseWorkspace(wxCommandEvent& event);
    void OnParseThreadScanIncludeCompleted(wxCommandEvent& event);
    void OnBuildProcessTerminated(clProcessEvent& event);
    void OnBuildProcessOutput(clProcessEvent& event);

protected:
    bool Load(const wxFileName& file);
    void DoOpen();
    void DoClose();
    void DoClear();
    void RestoreSession();

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

    /**
     * @brief close the workspace
     */
    void Close();

    /**
     * @brief save the workspace settings
     */
    void Save();

    /**
     * @brief parse the workspace
     */
    void Parse(bool fullParse);

    /**
     * @brief is this workspace opened?
     */
    bool IsOpen() const { return m_isLoaded; }

    /**
     * @brief return the compile flags strings
     */
    wxString GetCompileFlags() const;
    /**
     * @brief update the compile flags string
     */
    void SetCompileFlags(const wxString& compile_flags);

    void SetBuildTargets(const wxStringMap_t& buildTargets) { this->m_buildTargets = buildTargets; }
    const wxStringMap_t& GetBuildTargets() const { return m_buildTargets; }

    void SetFileExtensions(const wxString& fileExtensions)
    {
        this->m_fileExtensions = fileExtensions;
        m_fileScanNeeded = true; // require a new file caching
    }

    const wxString& GetFileExtensions() const { return m_fileExtensions; }
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_FS_SCAN_COMPLETED, clFileSystemEvent);
#endif // CLFILESYSTEMWORKSPACE_HPP
