#ifndef REMOTEWORKSPACE_HPP
#define REMOTEWORKSPACE_HPP

#include "IWorkspace.h" // Base class: IWorkspace

#define WORKSPACE_TYPE_NAME "Remote over SSH"

class RemoteWorkspace : public IWorkspace
{
    bool m_eventsConnected = false;

public:
    RemoteWorkspace();
    RemoteWorkspace(bool dummy);
    virtual ~RemoteWorkspace();

protected:
    void BindEvents();
    void UnbindEvents();

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

#endif // RemoteWorkspace_HPP
