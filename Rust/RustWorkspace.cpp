#include "RustWorkspace.hpp"

#include "clFileSystemWorkspace.hpp"
#include "file_logger.h"
#include "fileextmanager.h"

RustWorkspace::RustWorkspace() { SetWorkspaceType("Rust"); }

RustWorkspace::~RustWorkspace() {}

wxString RustWorkspace::GetFileName() const { return clFileSystemWorkspace::Get().GetFileName(); }
wxString RustWorkspace::GetDir() const { return clFileSystemWorkspace::Get().GetDir(); }
wxString RustWorkspace::GetFilesMask() const { return clFileSystemWorkspace::Get().GetFilesMask(); }
wxFileName RustWorkspace::GetProjectFileName(const wxString& projectName) const
{
    wxUnusedVar(projectName);
    return wxFileName();
}

void RustWorkspace::GetProjectFiles(const wxString& projectName, wxArrayString& files) const
{
    wxUnusedVar(projectName);
    wxUnusedVar(files);
}
wxString RustWorkspace::GetProjectFromFile(const wxFileName& filename) const { return wxEmptyString; }
void RustWorkspace::GetWorkspaceFiles(wxArrayString& files) const
{
    clFileSystemWorkspace::Get().GetWorkspaceFiles(files);
}
wxArrayString RustWorkspace::GetWorkspaceProjects() const { return {}; }

bool RustWorkspace::IsBuildSupported() const { return true; }
bool RustWorkspace::IsProjectSupported() const { return false; }

wxString RustWorkspace::GetName() const { return clFileSystemWorkspace::Get().GetName(); }

void RustWorkspace::SetProjectActive(const wxString& name) { wxUnusedVar(name); }

wxString RustWorkspace::GetDebuggerName() const { return clFileSystemWorkspace::Get().GetDebuggerName(); }

clEnvList_t RustWorkspace::GetEnvironment() const { return clFileSystemWorkspace::Get().GetEnvironment(); }
