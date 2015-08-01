#include "NodeJSDebuggerBreakpointManager.h"
#include "globals.h"
#include "imanager.h"
#include "ieditor.h"
#include "bookmark_manager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "NodeJSWorkspaceUserConfiguration.h"
#include "NoteJSWorkspace.h"
#include "bookmark_manager.h"
#include "macros.h"
#include <wx/stc/stc.h>
#include "ieditor.h"
#include <algorithm>

NodeJSBptManager::NodeJSBptManager()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &NodeJSBptManager::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &NodeJSBptManager::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSBptManager::OnEditorChanged, this);
}

NodeJSBptManager::~NodeJSBptManager()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &NodeJSBptManager::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &NodeJSBptManager::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSBptManager::OnEditorChanged, this);
}

void NodeJSBptManager::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();

    // Apply breakpoints for this editor
    if(clGetManager()) {
        IEditor* editor = clGetManager()->GetActiveEditor();
        if(editor) {
            NodeJSBreakpoint::List_t bps;
            if(GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps)) {
                NodeJSBreakpoint::List_t::iterator iter = bps.begin();
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
}

size_t NodeJSBptManager::GetBreakpointsForFile(const wxString& filename, NodeJSBreakpoint::List_t& bps) const
{
    bps.clear();
    NodeJSBreakpoint::List_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        if(iter->GetFilename() == filename) {
            bps.push_back(*iter);
        }
    }
    return bps.size();
}

void NodeJSBptManager::OnWorkspaceClosed(wxCommandEvent& event)
{
    event.Skip();

    // Save the breakpoints to the file system
    if(m_workspaceFile.IsOk() && m_workspaceFile.Exists()) {
        NodeJSWorkspaceUser userWorkspace(m_workspaceFile.GetFullPath());
        userWorkspace.Load().SetBreakpoints(m_breakpoints).Save();
        m_workspaceFile.Clear();
    }
}

void NodeJSBptManager::OnWorkspaceOpened(wxCommandEvent& event)
{
    event.Skip();
    m_workspaceFile.Clear();
    
    wxFileName fileName = event.GetString();
    if(FileExtManager::GetType(fileName.GetFullPath()) == FileExtManager::TypeWorkspaceNodeJS) {
        m_workspaceFile = fileName;
        NodeJSWorkspaceUser userConf(m_workspaceFile.GetFullPath());
        m_breakpoints = userConf.Load().GetBreakpoints();
    }
}

void NodeJSBptManager::SetBreakpoints(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    editor->GetCtrl()->MarkerDeleteAll(smt_breakpoint);

    NodeJSBreakpoint::List_t bps;
    GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps);
    NodeJSBreakpoint::List_t::const_iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        editor->GetCtrl()->MarkerAdd(iter->GetLine() - 1, smt_breakpoint);
    }
}

void NodeJSBptManager::AddBreakpoint(const wxFileName& filename, int line)
{
    NodeJSBreakpoint::List_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(iter == m_breakpoints.end()) {
        // new breakpoint
        NodeJSBreakpoint bp;
        bp.SetFilename(filename.GetFullPath());
        bp.SetLine(line);
        m_breakpoints.push_back(bp);
    }
}

void NodeJSBptManager::DeleteBreakpoint(const wxFileName& filename, int line)
{
    NodeJSBreakpoint::List_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(iter != m_breakpoints.end()) {
        m_breakpoints.erase(iter);
    }
}

bool NodeJSBptManager::HasBreakpoint(const wxFileName& filename, int line) const
{
    NodeJSBreakpoint::List_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    return iter != m_breakpoints.end();
}

const NodeJSBreakpoint& NodeJSBptManager::GetBreakpoint(const wxFileName& filename, int line) const
{
    static NodeJSBreakpoint nullBreakpoint;

    NodeJSBreakpoint::List_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(m_breakpoints.end() == iter) {
        return nullBreakpoint;
    }
    return *iter;
}

NodeJSBreakpoint& NodeJSBptManager::GetBreakpoint(const wxFileName& filename, int line)
{
    static NodeJSBreakpoint nullBreakpoint;

    NodeJSBreakpoint::List_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(m_breakpoints.end() == iter) {
        return nullBreakpoint;
    }
    return *iter;
}

void NodeJSBptManager::Save()
{
    // Save the breakpoints to the file system
    if(m_workspaceFile.Exists()) {
        NodeJSWorkspaceUser userWorkspace(m_workspaceFile.GetFullPath());
        userWorkspace.Load().SetBreakpoints(m_breakpoints).Save();
    }
}
