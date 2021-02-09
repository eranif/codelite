#ifndef RUSTWORKSPACE_HPP
#define RUSTWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace

class RustWorkspace : public IWorkspace
{
public:
    RustWorkspace();
    virtual ~RustWorkspace();

public:
    virtual wxString GetActiveProjectName() const { return wxEmptyString; }
    virtual wxFileName GetFileName() const;
    virtual wxString GetFilesMask() const;
    virtual wxFileName GetProjectFileName(const wxString& projectName) const;
    virtual void GetProjectFiles(const wxString& projectName, wxArrayString& files) const;
    virtual wxString GetProjectFromFile(const wxFileName& filename) const;
    virtual void GetWorkspaceFiles(wxArrayString& files) const;
    virtual wxArrayString GetWorkspaceProjects() const;
    virtual bool IsBuildSupported() const;
    virtual bool IsProjectSupported() const;
};

#endif // RUSTWORKSPACE_HPP
