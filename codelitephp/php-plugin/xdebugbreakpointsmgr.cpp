#include "xdebugbreakpointsmgr.h"
#include "PHPUserWorkspace.h"
#include "XDebugManager.h"
#include "globals.h"
#include "php_project.h"
#include "php_workspace.h"
#include "xdebugevent.h"
#include <algorithm>
#include <bookmark_manager.h>
#include <event_notifier.h>
#include <file_logger.h>
#include <plugin.h>
#include <wx/stc/stc.h>

XDebugBreakpointsMgr::XDebugBreakpointsMgr()
{
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_ENDED, &XDebugBreakpointsMgr::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_SESSION_STARTING, &XDebugBreakpointsMgr::OnXDebugSesstionStarting, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_LOADED, &XDebugBreakpointsMgr::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_WORKSPACE_CLOSED, &XDebugBreakpointsMgr::OnWorkspaceClosed, this);
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                  wxCommandEventHandler(XDebugBreakpointsMgr::OnEditorChanged), NULL, this);
}

XDebugBreakpointsMgr::~XDebugBreakpointsMgr()
{
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_ENDED, &XDebugBreakpointsMgr::OnXDebugSessionEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_SESSION_STARTING, &XDebugBreakpointsMgr::OnXDebugSesstionStarting, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_LOADED, &XDebugBreakpointsMgr::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_WORKSPACE_CLOSED, &XDebugBreakpointsMgr::OnWorkspaceClosed, this);
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(XDebugBreakpointsMgr::OnEditorChanged), NULL, this);
}

bool XDebugBreakpointsMgr::HasBreakpoint(const wxString& filename, int line) const
{
    if(std::find_if(m_breakpoints.begin(), m_breakpoints.end(), XDebugBreakpoint::Equal(filename, line)) ==
       m_breakpoints.end()) {
        return false;
    }
    return true;
}

void XDebugBreakpointsMgr::AddBreakpoint(const wxString& filename, int line)
{
    if(std::find_if(m_breakpoints.begin(), m_breakpoints.end(), XDebugBreakpoint::Equal(filename, line)) ==
       m_breakpoints.end()) {
        // new breakpoint
        m_breakpoints.push_back(XDebugBreakpoint(filename, line));
        Notify();
        Save();
    }
}

void XDebugBreakpointsMgr::DeleteBreakpoint(const wxString& filename, int line)
{
    XDebugBreakpoint::List_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), XDebugBreakpoint::Equal(filename, line));
    if(iter != m_breakpoints.end()) {
        m_breakpoints.erase(iter);
        Notify();
        Save();
    }
}

void XDebugBreakpointsMgr::OnXDebugSessionEnded(XDebugEvent& e)
{
    e.Skip();

    // clear any xdebug ID associated with the breakpoints
    for(auto& bp : m_breakpoints) {
        bp.SetBreakpointId(wxNOT_FOUND);
    }
}

bool XDebugBreakpointsMgr::GetBreakpoint(const wxString& filename, int line, XDebugBreakpoint& bp) const
{
    XDebugBreakpoint::List_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), XDebugBreakpoint::Equal(filename, line));
    if(iter != m_breakpoints.end()) {
        bp = *iter;
        return true;
    }
    return false;
}

bool XDebugBreakpointsMgr::GetBreakpoint(const wxString& filename, int line, XDebugBreakpoint& bp)
{
    XDebugBreakpoint::List_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), XDebugBreakpoint::Equal(filename, line));
    if(iter != m_breakpoints.end()) {
        bp = *iter;
        return true;
    }
    return false;
}

void XDebugBreakpointsMgr::OnXDebugSesstionStarting(XDebugEvent& e)
{
    e.Skip();
    // clear any xdebug ID associated with the breakpoints
    for(auto& bp : m_breakpoints) {
        bp.SetBreakpointId(wxNOT_FOUND);
    }
}

void XDebugBreakpointsMgr::OnWorkspaceClosed(PHPEvent& e)
{
    e.Skip();

    // Save the breakpoints to the file system
    if(!m_workspacePath.IsEmpty()) {
        PHPUserWorkspace userWorkspace(m_workspacePath);
        userWorkspace.Load().SetBreakpoints(m_breakpoints).Save();
        m_workspacePath.Clear();
    }
}

void XDebugBreakpointsMgr::OnWorkspaceOpened(PHPEvent& e)
{
    e.Skip();
    m_workspacePath = e.GetFileName();

    PHPUserWorkspace userWorkspace(m_workspacePath);
    m_breakpoints = userWorkspace.Load().GetBreakpoints();
}

void XDebugBreakpointsMgr::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();

    // Apply breakpoints for this editor
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    if(editor) {
        XDebugBreakpoint::List_t bps;
        if(GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps)) {
            XDebugBreakpoint::List_t::iterator iter = bps.begin();
            for(; iter != bps.end(); ++iter) {
                int markerMask = editor->GetCtrl()->MarkerGet(iter->GetLine() - 1);
                if(!(markerMask & mmt_breakpoint)) {
                    // No marker on this line yet
                    // add one
                    editor->GetCtrl()->MarkerAdd(iter->GetLine() - 1, smt_breakpoint);
                }
            }
        }
    }
}

size_t XDebugBreakpointsMgr::GetBreakpointsForFile(const wxString& filename, XDebugBreakpoint::List_t& bps) const
{
    bps.clear();
    XDebugBreakpoint::List_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        if(iter->GetFileName() == filename) {
            bps.push_back(*iter);
        }
    }
    return bps.size();
}

void XDebugBreakpointsMgr::Notify()
{
    PHPEvent e(wxEVT_XDEBUG_BREAKPOINTS_UPDATED);
    EventNotifier::Get()->AddPendingEvent(e);
}

void XDebugBreakpointsMgr::DeleteAllBreakpoints()
{
    m_breakpoints.clear();
    Notify();
    Save();
}

void XDebugBreakpointsMgr::Save()
{
    if(!m_workspacePath.IsEmpty()) {
        // Save the breakpoints to the file system
        PHPUserWorkspace userWorkspace(m_workspacePath);
        userWorkspace.Load().SetBreakpoints(m_breakpoints).Save();
    }
}
