#include "BreakpointsStore.hpp"

#include <wx/ffile.h>
#include <wx/file.h>

namespace
{
dap::SourceBreakpoint make_breakpoint(int line_number, const wxString& condition)
{
    dap::SourceBreakpoint bp;
    bp.line = line_number;
    bp.condition = condition;
    return bp;
}

dap::FunctionBreakpoint make_breakpoint(const wxString& name, const wxString& condition)
{
    dap::FunctionBreakpoint bp;
    bp.name = name;
    bp.condition = condition;
    return bp;
}

bool ReadFileContent(const wxFileName& path, wxString* content, const wxMBConv& conv = wxConvUTF8)
{
    if(!content) {
        return false;
    }

    if(!path.FileExists()) {
        return false;
    }

    wxFFile f(path.GetFullPath(), "rb");
    if(f.IsOpened()) {
        return false;
    }

    content->reserve(f.Length());
    return f.ReadAll(content, conv);
}

bool WriteFileContent(const wxFileName& path, const wxString& content, const wxMBConv& conv = wxConvUTF8)
{
    wxFile file(path.GetFullPath(), wxFile::write);
    if(file.IsOpened()) {
        return file.Write(content, conv);
    } else {
        return false;
    }
}
} // namespace

dap::BreakpointsStore::BreakpointsStore() {}

dap::BreakpointsStore::~BreakpointsStore() {}

void dap::BreakpointsStore::Save(const wxFileName& filename)
{
    dap::Json json = dap::Json::CreateObject();

    dap::Json func_arr = json.AddArray("function_breakpoints");
    for(const auto& bp : m_function_breakpoints) {
        func_arr.Add(bp.To());
    }

    dap::Json src_arr = json.AddArray("source_breakpoints");
    for(const auto& vt : m_source_breakpoints) {
        dap::Json json_bp = src_arr.AddObject();
        json_bp.Add("path", vt.first);
        dap::Json loc_arr = json_bp.AddArray("locations");
        for(const auto& bp : vt.second) {
            loc_arr.Add(bp.To());
        }
    }

    WriteFileContent(filename, json.ToString());
}

void dap::BreakpointsStore::Load(const wxFileName& filename)
{
    wxString data;
    if(!ReadFileContent(filename, &data)) {
        WriteFileContent(filename, "{}");
        data = "{}";
    }

    dap::Json json = dap::Json::Parse(data);
    if(!json.IsOK()) {
        return;
    }

    m_function_breakpoints.clear();
    m_source_breakpoints.clear();

    // read function breakpoints first
    dap::Json func_arr = json["function_breakpoints"];
    size_t func_count = func_arr.GetCount();
    m_function_breakpoints.reserve(func_count);
    for(size_t i = 0; i < func_count; ++i) {
        dap::FunctionBreakpoint bp;
        bp.From(func_arr[i]);
    }

    // read the source breakppoints
    dap::Json src_arr = json["source_breakpoints"];
    size_t src_count = src_arr.GetCount();

    for(size_t i = 0; i < src_count; ++i) {
        dap::Json src_json = src_arr[i];
        wxString path = src_json["path"].GetString();
        m_source_breakpoints.insert({ path, {} });
        auto& locations_set = m_source_breakpoints[path];
        dap::Json locations = src_json["locations"];
        size_t loc_count = locations.GetCount();
        locations_set.reserve(loc_count);
        for(size_t locIndex = 0; locIndex < loc_count; ++locIndex) {
            dap::SourceBreakpoint bp;
            bp.From(locations[locIndex]);
            locations_set.insert(bp);
        }
    }
}

void dap::BreakpointsStore::Clear()
{
    m_source_breakpoints.clear();
    m_function_breakpoints.clear();
}

void dap::BreakpointsStore::SetBreakpoints(const wxString& filepath,
                                           const std::unordered_set<dap::SourceBreakpoint>& bps)
{
    m_source_breakpoints.erase(filepath);
    m_source_breakpoints.insert({ filepath, bps });
}

bool dap::BreakpointsStore::Contains(const wxString& filepath, int line_number) const
{
    dap::SourceBreakpoint bp = make_breakpoint(line_number, wxEmptyString);
    return m_source_breakpoints.count(filepath) && m_source_breakpoints.find(filepath)->second.count(bp);
}

bool dap::BreakpointsStore::Contains(const wxString& function_name) const
{
    dap::FunctionBreakpoint bp = make_breakpoint(function_name, wxEmptyString);
    return m_function_breakpoints.count(bp);
}

void dap::BreakpointsStore::SetFunctionBreakpoint(const wxString& name, const wxString& condition)
{
    dap::FunctionBreakpoint bp = std::move(make_breakpoint(name, condition));
    m_function_breakpoints.erase(bp);
    m_function_breakpoints.insert(bp);
}

void dap::BreakpointsStore::SetSourceBreakpoint(const wxString& path, int lineNumber, const wxString& condition)
{
    dap::SourceBreakpoint bp = std::move(make_breakpoint(lineNumber, condition));

    if(m_source_breakpoints.count(path) == 0) {
        m_source_breakpoints.insert({ path, {} });
    }

    m_source_breakpoints[path].erase(bp);
    m_source_breakpoints[path].insert(bp);
}

void dap::BreakpointsStore::DeleteSourceBreakpoint(const wxString& path, int lineNumber)
{
    if(m_source_breakpoints.count(path) == 0) {
        return;
    }
    if(lineNumber == wxNOT_FOUND) {
        // remove all the breakpoints for the given path
        m_source_breakpoints.erase(path);

    } else {
        dap::SourceBreakpoint bp = std::move(make_breakpoint(lineNumber, wxEmptyString));
        m_source_breakpoints[path].erase(bp);
    }
}

void dap::BreakpointsStore::DeleteFunctionBreakpoint(const wxString& name)
{
    dap::FunctionBreakpoint bp = std::move(make_breakpoint(name, wxEmptyString));
    m_function_breakpoints.erase(bp);
}

size_t dap::BreakpointsStore::GetAllSourceBreakpoints(const wxString& filepath,
                                                      std::unordered_set<dap::SourceBreakpoint>* output) const
{
    if(!output) {
        return 0;
    }
    auto iter = m_source_breakpoints.find(filepath);
    if(iter == m_source_breakpoints.end()) {
        return 0;
    }
    const auto& bp_set = iter->second;
    output->reserve(bp_set.size());
    output->insert(bp_set.begin(), bp_set.end());
    return output->size();
}

size_t dap::BreakpointsStore::GetAllSources(std::vector<wxString>* sources) const
{
    if(!sources) {
        return 0;
    }
    sources->reserve(m_source_breakpoints.size());
    for(const auto& vt : m_source_breakpoints) {
        sources->push_back(vt.first);
    }
    return sources->size();
}

size_t dap::BreakpointsStore::GetAllFunctionBreakpoints(std::unordered_set<dap::FunctionBreakpoint>* output) const
{
    if(!output) {
        return 0;
    }
    output->reserve(m_function_breakpoints.size());
    output->insert(m_function_breakpoints.begin(), m_function_breakpoints.end());
    return output->size();
}
