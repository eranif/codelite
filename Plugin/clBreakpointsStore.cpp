#include "clBreakpointsStore.hpp"

#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "imanager.h"

clBreakpointsStore::clBreakpointsStore() {}

clBreakpointsStore::~clBreakpointsStore() {}

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

void clBreakpointsStore::Clear()
{
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

bool clBreakpointsStore::HasSourceBreakpoint(const wxString& path, int lineNumber) const
{
    if(m_source_breakpoints.count(path) == 0) {
        return false;
    }

    auto bps = m_source_breakpoints.find(path)->second;
    UIBreakpoint find_what{ path, lineNumber };
    return bps.count(find_what) != 0;
}

void clBreakpointsStore::AddSourceBreakpoint(const wxString& path, int lineNumber)
{
    if(HasSourceBreakpoint(path, lineNumber)) {
        return;
    }
    if(m_source_breakpoints.count(path) == 0) {
        m_source_breakpoints.insert({ path, {} });
    }
    m_source_breakpoints[path].insert({ path, lineNumber });
}

void clBreakpointsStore::DeleteSourceBreakpoint(const wxString& path, int lineNumber)
{
    if(m_source_breakpoints.count(path) == 0) {
        return;
    }

    if(lineNumber == wxNOT_FOUND) {
        m_source_breakpoints.erase(path);
    } else {
        if(!HasSourceBreakpoint(path, lineNumber)) {
            return;
        }
        m_source_breakpoints[path].erase({ path, lineNumber });
    }
}

bool clBreakpointsStore::HasBreakpoint(const UIBreakpoint& bp) const
{
    if(bp.IsFunctionBreakpoint()) {
        return m_function_breakpoints.count(bp);
    } else if(bp.IsSourceBreakpoint()) {
        const wxString& path = bp.GetFile();
        return m_source_breakpoints.count(path) != 0 && m_source_breakpoints.find(path)->second.count(bp) != 0;
    } else {
        return false;
    }
}

void clBreakpointsStore::AddBreakpoint(const UIBreakpoint& bp)
{
    if(HasBreakpoint(bp)) {
        return;
    }
    if(bp.IsFunctionBreakpoint()) {
        m_function_breakpoints.insert({ bp, {} });
    } else if(bp.IsSourceBreakpoint()) {
        AddSourceBreakpoint(bp.GetFile(), bp.GetLine());
    }
}
