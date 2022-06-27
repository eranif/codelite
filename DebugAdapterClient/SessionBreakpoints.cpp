#include "SessionBreakpoints.hpp"

#include <algorithm>

SessionBreakpoints::SessionBreakpoints() {}

SessionBreakpoints::~SessionBreakpoints() {}

void SessionBreakpoints::clear() { m_breakpoints.clear(); }

int SessionBreakpoints::find_by_id_internal(int id)
{
    for(size_t i = 0; i < m_breakpoints.size(); ++i) {
        if(m_breakpoints[i].id == id) {
            return i;
        }
    }
    return wxNOT_FOUND;
}

bool SessionBreakpoints::find_by_id(int id, dap::Breakpoint* bp)
{
    if(!bp) {
        return false;
    }

    int index = find_by_id_internal(id);
    if(index == wxNOT_FOUND)
        return false;
    *bp = m_breakpoints[index];
    return true;
}

size_t SessionBreakpoints::find_by_path(const wxString& path, std::vector<dap::Breakpoint>* bps)
{
    if(!bps) {
        return 0;
    }

    for(const auto& bp : m_breakpoints) {
        if(bp.source.path == path) {
            bps->push_back(bp);
        }
    }
    return bps->size();
}

bool SessionBreakpoints::update_or_insert(const dap::Breakpoint& bp)
{
    if(bp.id <= 0) {
        return false;
    }

    int where = find_by_id_internal(bp.id);
    if(where == wxNOT_FOUND) {
        m_breakpoints.push_back(bp);
    } else {
        // only update when the bp is changing from not-verified -> verified
        if(bp.verified) {
            m_breakpoints[where] = bp;
        }
    }
    return true;
}

void SessionBreakpoints::bulk_update(const std::vector<dap::Breakpoint>& list)
{
    for(const auto& bp : list) {
        update_or_insert(bp);
    }
}

size_t SessionBreakpoints::find_by_path_internal(const wxString& path, std::vector<dap::Breakpoint>* bps)
{
    if(path.empty() || !bps) {
        return 0;
    }

    bps->reserve(m_breakpoints.size());
    for(size_t i = 0; i < m_breakpoints.size(); ++i) {
        if(m_breakpoints[i].source.path == path) {
            bps->push_back(m_breakpoints[i]);
        }
    }
    return bps->size();
}

void SessionBreakpoints::delete_by_id(int id)
{
    int index = find_by_id_internal(id);
    if(index == wxNOT_FOUND) {
        return;
    }
    m_breakpoints.erase(m_breakpoints.begin() + index);
}

void SessionBreakpoints::delete_by_path(const wxString& path)
{
    std::vector<dap::Breakpoint> bps;
    find_by_path_internal(path, &bps);

    for(const auto& bp : bps) {
        delete_by_id(bp.id);
    }
}

void SessionBreakpoints::delete_by_paths(const std::vector<dap::Breakpoint>& bps)
{
    std::unordered_set<wxString> files;
    for(const auto& bp : bps) {
        if(!bp.source.path.empty()) {
            files.insert(bp.source.path);
        }
    }

    for(const auto& path : files) {
        delete_by_path(path);
    }
}
