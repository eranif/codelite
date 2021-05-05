#include "NodeJSDebuggerBreakpointManager.h"
#include "NodeJSEvents.h"
#include "NodeJSWorkspaceUserConfiguration.h"
#include "NoteJSWorkspace.h"
#include "bookmark_manager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "macros.h"
#include <algorithm>
#include <wx/stc/stc.h>

NodeJSBptManager::NodeJSBptManager()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &NodeJSBptManager::OnWorkspaceOpened, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &NodeJSBptManager::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSBptManager::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_NODEJS_DEBUGGER_STOPPED, &NodeJSBptManager::OnDebuggerStopped, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &NodeJSBptManager::OnFileSaved, this);
}

NodeJSBptManager::~NodeJSBptManager()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &NodeJSBptManager::OnWorkspaceOpened, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &NodeJSBptManager::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &NodeJSBptManager::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_NODEJS_DEBUGGER_STOPPED, &NodeJSBptManager::OnDebuggerStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &NodeJSBptManager::OnFileSaved, this);
}

void NodeJSBptManager::OnEditorChanged(wxCommandEvent& e)
{
    e.Skip();

    // Apply breakpoints for this editor
    if(clGetManager()) {
        IEditor* editor = clGetManager()->GetActiveEditor();
        if(editor) {
            NodeJSBreakpoint::Vec_t bps;
            if(GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps)) {
                NodeJSBreakpoint::Vec_t::iterator iter = bps.begin();
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

size_t NodeJSBptManager::GetBreakpointsForFile(const wxString& filename, NodeJSBreakpoint::Vec_t& bps) const
{
    bps.clear();
    NodeJSBreakpoint::Vec_t::const_iterator iter = m_breakpoints.begin();
    for(; iter != m_breakpoints.end(); ++iter) {
        if(iter->GetFilename() == filename) { bps.push_back(*iter); }
    }
    return bps.size();
}

void NodeJSBptManager::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();

    // Save the breakpoints to the file system
    if(m_workspaceFile.IsOk() && m_workspaceFile.Exists()) {
        NodeJSWorkspaceUser userWorkspace(m_workspaceFile.GetFullPath());
        userWorkspace.Load().SetBreakpoints(m_breakpoints).Save();
        m_workspaceFile.Clear();
    }
}

void NodeJSBptManager::OnWorkspaceOpened(clWorkspaceEvent& event)
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

    NodeJSBreakpoint::Vec_t bps;
    GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps);
    NodeJSBreakpoint::Vec_t::const_iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        editor->GetCtrl()->MarkerAdd(iter->GetLine() - 1, smt_breakpoint);
    }
}

void NodeJSBptManager::AddBreakpoint(const wxFileName& filename, int line)
{
    NodeJSBreakpoint::Vec_t::const_iterator iter =
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
    NodeJSBreakpoint::Vec_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(iter != m_breakpoints.end()) { m_breakpoints.erase(iter); }
}

bool NodeJSBptManager::HasBreakpoint(const wxFileName& filename, int line) const
{
    NodeJSBreakpoint::Vec_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    return iter != m_breakpoints.end();
}

const NodeJSBreakpoint& NodeJSBptManager::GetBreakpoint(const wxFileName& filename, int line) const
{
    static NodeJSBreakpoint nullBreakpoint;

    NodeJSBreakpoint::Vec_t::const_iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(m_breakpoints.end() == iter) { return nullBreakpoint; }
    return *iter;
}

NodeJSBreakpoint& NodeJSBptManager::GetBreakpoint(const wxFileName& filename, int line)
{
    static NodeJSBreakpoint nullBreakpoint;

    NodeJSBreakpoint::Vec_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetFilename() == filename.GetFullPath() && bp.GetLine() == line) return true;
            return false;
        });
    if(m_breakpoints.end() == iter) { return nullBreakpoint; }
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

void NodeJSBptManager::OnDebuggerStopped(clDebugEvent& event)
{
    event.Skip();
    // Clear the node's breakpoint ID
    std::for_each(m_breakpoints.begin(), m_breakpoints.end(), [&](NodeJSBreakpoint& bp) { bp.SetNodeBpID(""); });
}

void NodeJSBptManager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();
    if(!NodeJSWorkspace::Get()->IsOpen()) { return; }
    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Delete all the breakpoints belonged to this file
    wxArrayString nodeBpIds;
    bool isDebuggerRunning = NodeJSWorkspace::Get()->GetDebugger()->IsRunning();
    while(true) {
        NodeJSBreakpoint::Vec_t::iterator iter =
            std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
                if(bp.GetFilename() == editor->GetFileName().GetFullPath()) return true;
                return false;
            });
        if(iter == m_breakpoints.end()) { break; }
        if(isDebuggerRunning) {
            const NodeJSBreakpoint& bp = *iter;
            nodeBpIds.Add(bp.GetNodeBpID());
        }
        m_breakpoints.erase(iter);
    }

    // Tell Node to delete these breakpoints
    if(isDebuggerRunning) {
        // Remove all the breakpoints from the debugger
        for(size_t i = 0; i < nodeBpIds.size(); ++i) {
            NodeJSWorkspace::Get()->GetDebugger()->DeleteBreakpointByID(nodeBpIds.Item(i));
        }
    }

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    int lineNo = ctrl->MarkerNext(0, mmt_breakpoint);
    while(lineNo != wxNOT_FOUND) {
        // stc uses 0 based index
        ++lineNo;
        AddBreakpoint(editor->GetFileName(), lineNo);
        if(isDebuggerRunning) {
            const NodeJSBreakpoint& bp = GetBreakpoint(editor->GetFileName(), lineNo);
            NodeJSWorkspace::Get()->GetDebugger()->SetBreakpoint(bp.GetFilename(), bp.GetLine());
        }
        // find the next breakpoint
        lineNo = ctrl->MarkerNext(lineNo, mmt_breakpoint);
    }
}

void NodeJSBptManager::DeleteAll()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(),
                  [&](IEditor* editor) { editor->GetCtrl()->MarkerDeleteAll(smt_breakpoint); });
    m_breakpoints.clear();

    // Tell the UI to refresh its view
    clDebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
    EventNotifier::Get()->AddPendingEvent(bpEvent);
}

wxArrayString NodeJSBptManager::GetAllAppliedBreakpoints() const
{
    wxArrayString arr;
    std::for_each(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
        if(!bp.GetNodeBpID().IsEmpty()) { arr.Add(bp.GetNodeBpID()); }
    });
    return arr;
}

void NodeJSBptManager::DeleteByID(const wxString& bpid)
{
    NodeJSBreakpoint::Vec_t::iterator iter =
        std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [&](const NodeJSBreakpoint& bp) {
            if(bp.GetNodeBpID() == bpid) return true;
            return false;
        });
    if(iter == m_breakpoints.end()) { return; }
    const NodeJSBreakpoint& bp = *iter;
    IEditor* editor = clGetManager()->FindEditor(bp.GetFilename());
    DeleteBreakpoint(bp.GetFilename(), bp.GetLine());

    if(editor) {
        // Update the breakpoints set on this editor
        SetBreakpoints(editor);
    }
    // Tell the UI to refresh its view
    clDebugEvent bpEvent(wxEVT_NODEJS_DEBUGGER_UPDATE_BREAKPOINTS_VIEW);
    EventNotifier::Get()->AddPendingEvent(bpEvent);
}
