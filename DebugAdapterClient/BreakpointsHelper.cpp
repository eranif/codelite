#include "BreakpointsHelper.hpp"

#include "event_notifier.h"

#include <algorithm>

namespace
{
// helper methods converting between clDebuggerBreakpoint and dap::XXXBreakpoint
bool is_source_breakpoint(const clDebuggerBreakpoint& bp)
{
    return bp.bp_type == BreakpointType::BP_type_break && !bp.file.empty() && bp.lineno > 0;
}

bool is_function_breakpoint(const clDebuggerBreakpoint& bp)
{
    return bp.bp_type == BreakpointType::BP_type_break && !bp.function_name.empty();
}

dap::SourceBreakpoint to_dap_source_bp(const clDebuggerBreakpoint& bp)
{
    dap::SourceBreakpoint d;
    d.line = bp.lineno;
    d.condition = bp.conditions;
    return d;
}

dap::FunctionBreakpoint to_dap_function_bp(const clDebuggerBreakpoint& bp)
{
    dap::FunctionBreakpoint d;
    d.name = bp.function_name;
    d.condition = bp.conditions;
    return d;
}

} // namespace

BreakpointsHelper::BreakpointsHelper(dap::Client& client, const DebugSession& session, clModuleLogger& log)
    : m_client(client)
    , m_session(session)
    , LOG(log)
{
    // create a snapshot of the breakpoints manager
    clDebuggerBreakpoint::Vec_t all_bps;
    clGetManager()->GetAllBreakpoints(all_bps);

    for(const auto& bp : all_bps) {
        if(bp.file.empty() || bp.lineno <= 0)
            continue;

        if(m_ui_breakpoints.count(bp.file) == 0) {
            m_ui_breakpoints.insert({ bp.file, {} });
        }
        m_ui_breakpoints[bp.file].push_back(bp);
    }

    clGetManager()->DeleteAllBreakpoints();
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &BreakpointsHelper::OnToggleBreakpoint, this);
}

BreakpointsHelper::~BreakpointsHelper()
{
    // restore the breakpoints
    clDebuggerBreakpoint::Vec_t all_bps;
    for(const auto& vt : m_ui_breakpoints) {
        LOG_DEBUG(LOG) << "Restoring breakpoints for file:" << vt.first << " -" << vt.second.size() << "breakpoints"
                       << endl;
        for(const auto& bp : vt.second) {
            all_bps.push_back(bp);
        }
    }

    clGetManager()->SetBreakpoints(all_bps);
    LOG_DEBUG(LOG) << "Restoring breakpoints...done" << endl;

    // refresh markers
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &BreakpointsHelper::OnToggleBreakpoint, this);
}

void BreakpointsHelper::OnToggleBreakpoint(clDebugEvent& event)
{
    // this instance only exists while an active debug session is running
    // during such a session, we capture all the UI breakpoints adding/deleting
    event.Skip();
    LOG_DEBUG(LOG) << "Toggle breakpoint called for:" << event.GetFileName() << ":" << event.GetLineNumber() << endl;
    if(m_ui_breakpoints.count(event.GetFileName()) == 0) {
        m_ui_breakpoints.insert({ event.GetFileName(), {} });
    }

    auto& file_breakpoints = m_ui_breakpoints[event.GetFileName()];
    auto iter = std::find_if(file_breakpoints.begin(), file_breakpoints.end(), [&](const clDebuggerBreakpoint& bp) {
        return bp.file == event.GetFileName() && bp.lineno == event.GetLineNumber();
    });

    if(iter == file_breakpoints.end()) {
        auto bp = clGetManager()->CreateBreakpoint(event.GetFileName(), event.GetLineNumber());
        file_breakpoints.push_back(bp);
        LOG_DEBUG(LOG) << "Breakpoint does not exist - adding it" << endl;
    } else {
        file_breakpoints.erase(iter);
        LOG_DEBUG(LOG) << "Breakpoint exists - deleting it" << endl;
    }

    // need to apply the breakpoints for this file
    ApplyBreakpoints(event.GetFileName());
}

void BreakpointsHelper::ApplyBreakpoints(const wxString& path)
{
    if(!m_client.IsConnected()) {
        return;
    }
    LOG_DEBUG(LOG) << "Applying breakpoints" << endl;
    std::unordered_map<wxString, std::vector<dap::SourceBreakpoint>> dap_source_breakpoints;
    std::vector<dap::FunctionBreakpoint> dap_function_breakpoints;

    if(m_ui_breakpoints.empty()) {
        return;
    }

    dap_source_breakpoints.reserve(m_ui_breakpoints.size());
    dap_function_breakpoints.reserve(m_ui_breakpoints.size());

    for(const auto& vt : m_ui_breakpoints) {
        if(vt.second.empty()) {
            // no breakpoints for this source file, we need to pass an empty array to dap
            dap_source_breakpoints.insert({ vt.first, {} });

        } else {
            for(const auto& bp : vt.second) {
                if(!is_source_breakpoint(bp)) {
                    continue;
                }
                if(path.empty() || path == bp.file) {
                    // all breakpoints
                    if(dap_source_breakpoints.count(bp.file) == 0) {
                        dap_source_breakpoints.insert({ bp.file, {} });
                    }
                    dap_source_breakpoints[bp.file].push_back(to_dap_source_bp(bp));
                }
            }
        }
    }

    // dont pass empty array, it will tell dap to clear all breakpoints
    for(const auto& vt : dap_source_breakpoints) {
        wxFileName filepath(vt.first);
        LOG_DEBUG(LOG) << "Applying breakpoints for file:" << filepath << endl;
        for(const auto& bp : vt.second) {
            LOG_DEBUG(LOG) << "Line:" << bp.line << ". Condition: " << bp.condition << endl;
        }
        wxString source_path = NormalisePathForSend(filepath.GetFullPath());
        m_client.SetBreakpointsFile(source_path, vt.second);
    }

    if(!dap_function_breakpoints.empty()) {
        LOG_DEBUG(LOG) << "Applying function breakpoints:" << endl;
        for(const auto& bp : dap_function_breakpoints) {
            LOG_DEBUG(LOG) << "Function name:" << bp.name << ". Condition: " << bp.condition << endl;
        }
        m_client.SetFunctionBreakpoints(dap_function_breakpoints);
    }
}

wxString BreakpointsHelper::NormalisePathForSend(const wxString& path) const
{
    wxFileName fn(path);

    // easy path
    if(m_session.dap_server.UseRelativePath()) {
        return fn.GetFullName();
    }

    const auto& dap = m_session.dap_server;

    // attempt to make it fullpath
    if(fn.IsRelative() && !dap.UseRelativePath()) {
        fn.MakeAbsolute(m_session.working_directory);
    }

    if(!dap.UseVolume()) {
        // no volume
        fn.SetVolume(wxEmptyString);
    }

    wxString fullpath = fn.GetFullPath();
    if(dap.UseForwardSlash()) {
        fullpath.Replace(R"(\)", "/");
    }
    return fullpath;
}
