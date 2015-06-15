#ifndef IWORKSPACE_H
#define IWORKSPACE_H

#include <wx/string.h>
#include <wx/filename.h>
#include <list>
#include <wx/event.h>

/**
 * @class IWorkspace
 * @brief an interface representing the workspace concept of CodeLite
 * Each plugin that wants to implement a workspace must implement this interface
 */
class IWorkspace : public wxEvtHandler
{
protected:
    wxString m_workspaceType;
    
public:
    typedef std::list<IWorkspace*> List_t;
    
public:
    IWorkspace() {}
    virtual ~IWorkspace() {}

    /**
     * @brief set the workspace type
     * For example: "C++ Workspace", "PHP Workspace" etc
     */
    void SetWorkspaceType(const wxString& type) { this->m_workspaceType = type; }

    /**
     * @brief return the workspace name
     */
    const wxString& GetWorkspaceType() const { return m_workspaceType; }

    /**
     * @brief is this workspace support the build concept?
     * e.g. for C++ workspace, the answer is 'true', for PHP workspace, this will be 'false'
     */
    virtual bool IsBuildSupported() const = 0;

    /**
     * @brief is this workspace support the project concept?
     */
    virtual bool IsProjectSupported() const = 0;
    
    /**
     * @brief return the file masking for this workspace
     */
    virtual wxString GetFilesMask() const = 0;
};

#endif // IWORKSPACE_H
