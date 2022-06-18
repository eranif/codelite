#include "clBreakpointsStore.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"

clBreakpointsStore::clBreakpointsStore()
{
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &clBreakpointsStore::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &clBreakpointsStore::OnWorkspaceClosed, this);
}

clBreakpointsStore::~clBreakpointsStore()
{
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &clBreakpointsStore::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &clBreakpointsStore::OnWorkspaceClosed, this);
}

void clBreakpointsStore::Save(const wxFileName& filename)
{
    JSON root(cJSON_Array);
    auto json = root.toElement();

    // for function breakpoints, we only store the
    // the memory one, not the resolved one
    for(const auto& vt : m_function_breakpoints) {
        json.append(vt.first.To());
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
            m_function_breakpoints.insert({ bp, {} });
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

void clBreakpointsStore::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
    m_breakpoint_file = event.GetFileName();
    m_breakpoint_file.AppendDir(".codelite");
    m_breakpoint_file.SetFullName("breakpoints.json");
    if(!m_breakpoint_file.FileExists()) {
        // first time, create the file with empty content
        m_breakpoint_file.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        FileUtils::WriteFileContent(m_breakpoint_file, "[]");
    }
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

void clBreakpointsStore::SetBreakpoints(const wxString& filepath, const UIBreakpoint::set_t& bps)
{
    m_source_breakpoints.erase(filepath);
    m_source_breakpoints.insert({ filepath, bps });
}

void clBreakpointsStore::ResolveFunctionBreakpoint(const UIBreakpoint& func_bp, const UIBreakpoint& bp)
{
    if(m_function_breakpoints.count(func_bp) == 0) {
        return;
    }

    m_function_breakpoints.erase(func_bp);
    m_function_breakpoints.insert({ func_bp, bp });
}

size_t clBreakpointsStore::GetAllSourceBreakpoints(UIBreakpoint::set_t* output, const wxString& filepath) const
{
    if(!output) {
        return 0;
    }

    if(filepath.empty()) {
        // return all breakpoints from all files
        for(const auto& vt : m_source_breakpoints) {
            output->insert(vt.second.begin(), vt.second.end());
        }
    } else if(m_source_breakpoints.count(filepath)) {
        // this file exists
        const auto& bps = m_source_breakpoints.find(filepath)->second;
        output->reserve(bps.size());
        output->insert(bps.begin(), bps.end());
    } else {
        return 0;
    }
    return output->size();
}
