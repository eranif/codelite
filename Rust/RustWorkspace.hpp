#ifndef RUSTWORKSPACE_HPP
#define RUSTWORKSPACE_HPP

#include "clWorkspaceManager.h"

class RustWorkspace : public LocalWorkspaceCommon
{
public:
    RustWorkspace();
    ~RustWorkspace() override = default;

public:
    wxString GetActiveProjectName() const override { return wxEmptyString; }
    wxString GetFileName() const override;
    wxString GetDir() const override;
    wxString GetFilesMask() const override;
    wxFileName GetProjectFileName(const wxString& projectName) const override;
    void GetProjectFiles(const wxString& projectName, wxArrayString& files) const override;
    wxString GetProjectFromFile(const wxFileName& filename) const override;
    void GetWorkspaceFiles(wxArrayString& files) const override;
    wxArrayString GetWorkspaceProjects() const override;
    bool IsBuildSupported() const override;
    bool IsProjectSupported() const override;
    wxString GetName() const override;
    void SetProjectActive(const wxString& name) override;
    wxString GetDebuggerName() const override;
    clEnvList_t GetEnvironment() const override;
};

#endif // RUSTWORKSPACE_HPP
