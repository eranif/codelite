#include "clDebuggerBreakpointStore.hpp"
#include "file_logger.h"

clDebuggerBreakpointStore::clDebuggerBreakpointStore() {}

clDebuggerBreakpointStore::~clDebuggerBreakpointStore() {}

void clDebuggerBreakpointStore::Save(const wxFileName& filename)
{
    JSON root(cJSON_Array);
    auto json = root.toElement();
    for(const auto& bp : m_breakpoints) {
        json.arrayAppend(bp.ToJSON());
    }
    clDEBUG() << "Storing breakpoints to" << filename << clEndl;
    root.save(filename);
}

void clDebuggerBreakpointStore::Load(const wxFileName& filename)
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

    m_breakpoints.clear();
    size_t size = json.arraySize();
    if(size) {
        m_breakpoints.reserve(size);
        for(size_t i = 0; i < size; ++i) {
            clDebuggerBreakpoint bp;
            bp.FromJSON(json[i]);
            m_breakpoints.push_back(bp);
        }
    }
    clDEBUG() << "Loaded" << size << "breakpoints from file:" << filename << clEndl;
}
