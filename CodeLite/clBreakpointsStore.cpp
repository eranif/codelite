#include "clBreakpointsStore.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"

clBreakpointsStore::clBreakpointsStore()
{
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_BREAKPOINT_ADDED, &clBreakpointsStore::OnBreakpointAdded, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_BREAKPOINT_DELETED, &clBreakpointsStore::OnBreakpointDeleted, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_STARTED, &clBreakpointsStore::OnDebugSessionStarted, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &clBreakpointsStore::OnDebugSessionEnded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBreakpointsStore::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clBreakpointsStore::OnWorkspaceClosed, this);
}

clBreakpointsStore::~clBreakpointsStore()
{
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_BREAKPOINT_ADDED, &clBreakpointsStore::OnBreakpointAdded, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_BREAKPOINT_DELETED, &clBreakpointsStore::OnBreakpointDeleted, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_STARTED, &clBreakpointsStore::OnDebugSessionStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &clBreakpointsStore::OnDebugSessionEnded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBreakpointsStore::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clBreakpointsStore::OnWorkspaceClosed, this);
}

void clBreakpointsStore::Save(const wxFileName& filename)
{
    JSON root(cJSON_Array);
    auto json = root.toElement();

    for(const auto& bp : m_function_breakpoints) {
        json.append(bp.To());
    }

    for(const auto& vt : m_source_breakpoints) {
        for(const auto& bp : vt.second) {
            json.append(bp.To());
        }
    }
    clDEBUG() << "Storing breakpoints to" << filename << clEndl;
    root.save(filename);
}

void clBreakpointsStore::Load(const wxFileName& filename)
{
    JSON root(filename);
    if(!root.isOk()) {
        clDEBUG() << "Failed to load breakpoints from file:" << filename << clEndl;
        return;
    }

    auto json = root.toElement();
    if(!json.isArray()) {
        clWARNING() << "Failed to load breakpoints from file:" << filename << ": expected array" << clEndl;
        return;
    }

    m_function_breakpoints.clear();
    m_source_breakpoints.clear();

    size_t count = json.arraySize();
    size_t func_breakpoints_count = 0;
    size_t source_breakpoints_count = 0;
    for(size_t i = 0; i < count; ++i) {
        UIBreakpoint bp;
        bp.From(json[i]);
        if(bp.IsFunctionBreakpoint()) {
            m_function_breakpoints.insert(bp);
            func_breakpoints_count++;
        } else if(bp.IsSourceBreakpoint()) {
            const wxString& file = bp.GetFile();
            if(m_source_breakpoints.count(file) == 0) {
                m_source_breakpoints.insert({ file, {} });
            }
            m_source_breakpoints[file].insert(bp);
            source_breakpoints_count++;
        }
    }

    clDEBUG() << "Loaded" << source_breakpoints_count << "source breakpoints and" << func_breakpoints_count
              << "function breakpoints from" << filename << endl;
}

void clBreakpointsStore::OnBreakpointAdded(clDebugEvent& event)
{
    event.Skip();
    const auto& bp = event.GetUiBreakpoint();
    if(bp.IsFunctionBreakpoint()) {
        m_function_breakpoints.insert(bp);

    } else if(bp.IsSourceBreakpoint()) {
        const wxString& file = bp.GetFile();
        if(m_source_breakpoints.count(file) == 0) {
            m_source_breakpoints.insert({ file, {} });
        }
        m_source_breakpoints[file].insert(bp);
    }
}

void clBreakpointsStore::OnBreakpointDeleted(clDebugEvent& event)
{
    event.Skip();
    const auto& bp = event.GetUiBreakpoint();
    if(bp.IsFunctionBreakpoint()) {
        m_function_breakpoints.erase(bp);

    } else if(bp.IsSourceBreakpoint()) {
        const wxString& file = bp.GetFile();
        if(m_source_breakpoints.count(file) == 0) {
            return;
        }
        m_source_breakpoints[file].erase(bp);
    }
}

void clBreakpointsStore::OnDebugSessionStarted(clDebugEvent& event)
{
    event.Skip();
    m_debug_session_in_progress = true;
}

void clBreakpointsStore::OnDebugSessionEnded(clDebugEvent& event)
{
    event.Skip();
    m_debug_session_in_progress = false;
}

void clBreakpointsStore::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
    m_breakpoint_file = event.GetFileName();
    if(!m_breakpoint_file.IsOk() || !m_breakpoint_file.FileExists()) {
        Clear();
        return;
    }

    m_breakpoint_file.AppendDir(".codelite");
    m_breakpoint_file.SetFullName("breakpoints.json");
    Load(m_breakpoint_file);
}

void clBreakpointsStore::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Save(m_breakpoint_file);
    Clear();
}

void clBreakpointsStore::Clear()
{
    m_breakpoint_file.Clear();
    m_source_breakpoints.clear();
    m_function_breakpoints.clear();
}