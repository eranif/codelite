#ifndef NODEJSDEBUGGERBREAKPOINTMANAGER_H
#define NODEJSDEBUGGERBREAKPOINTMANAGER_H

#include "NodeJSDebuggerBreakpoint.h"
#include <wx/event.h>

class IEditor;
class NodeJSBptManager : public wxEvtHandler
{
    NodeJSBreakpoint::List_t m_breakpoints;
    wxFileName m_workspaceFile;

protected:
    void OnEditorChanged(wxCommandEvent& e);
    void OnWorkspaceOpened(wxCommandEvent& event);
    void OnWorkspaceClosed(wxCommandEvent& event);

public:
    NodeJSBptManager();
    virtual ~NodeJSBptManager();
    void Save();
    
    /**
     * @brief return breakpoints for a given file
     */
    size_t GetBreakpointsForFile(const wxString& filename, NodeJSBreakpoint::List_t& bps) const;

    /**
     * @brief set all breakpoints for the given file
     */
    void SetBreakpoints(IEditor* editor);
    /**
     * @brief do we have a breakpoint for a given file and line?
     */
    bool HasBreakpoint(const wxFileName& filename, int line) const;
    /**
     * @brief get the breakpoint for a give file and line
     */
    const NodeJSBreakpoint& GetBreakpoint(const wxFileName& filename, int line) const;
    NodeJSBreakpoint& GetBreakpoint(const wxFileName& filename, int line);
    /**
     * @brief delete a breakpoint by file and line
     */
    void DeleteBreakpoint(const wxFileName& filename, int line);
    /**
     * @brief add breakpoint by file and line
     */
    void AddBreakpoint(const wxFileName& filename, int line);
    
    const NodeJSBreakpoint::List_t& GetBreakpoints() const { return m_breakpoints; }
};

#endif // NODEJSDEBUGGERBREAKPOINTMANAGER_H
