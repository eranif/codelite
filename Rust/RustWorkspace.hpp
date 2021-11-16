#ifndef RUSTWORKSPACE_HPP
#define RUSTWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace

class RustWorkspace : public IWorkspace
{
public:
    RustWorkspace();
    virtual ~RustWorkspace();

public:
    wxString GetActiveProjectName() const override { return wxEmptyString; }
    wxFileName GetFileName() const override;
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
};

#endif // RUSTWORKSPACE_HPP
