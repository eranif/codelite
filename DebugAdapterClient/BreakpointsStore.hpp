#ifndef CLDEBUGGERBREAKPOINTSTORE_HPP
#define CLDEBUGGERBREAKPOINTSTORE_HPP

#include "dap/dap.hpp"

#include <unordered_set>
#include <vector>
#include <wx/filename.h>

namespace dap
{
class BreakpointsStore
{
    // breakpoints as set by the user
    std::unordered_map<wxString, std::unordered_set<dap::SourceBreakpoint>> m_source_breakpoints;
    std::unordered_set<dap::FunctionBreakpoint> m_function_breakpoints;

public:
    BreakpointsStore();
    virtual ~BreakpointsStore();

    /**
     * @brief clear the store
     */
    void Clear();

    /**
     * @brief store the breakpoints into a file
     */
    void Save(const wxFileName& filename);

    /**
     * @brief load list of breakpoints from the file system
     */
    void Load(const wxFileName& filename);

    /**
     * @brief update the breakpoints for a given source
     */
    void SetBreakpoints(const wxString& filepath, const std::unordered_set<dap::SourceBreakpoint>& bps);

    /**
     * @brief return true if the store contains `bp`
     */
    bool Contains(const wxString& filepath, int line_number) const;

    /**
     * @brief return true if the store contains `bp`
     */
    bool Contains(const wxString& function_name) const;

    /**
     * @brief add function breakpoint. If similar breakpoint exists, replace it
     */
    void SetFunctionBreakpoint(const wxString& name, const wxString& condition = wxEmptyString);

    /**
     * @brief add source breakpoint. If similar breakpoint exists, replace it
     */
    void SetSourceBreakpoint(const wxString& path, int lineNumber, const wxString& condition = wxEmptyString);

    /**
     * @brief delete source breakpoint
     * if lineNumber is wxNOT_FOUND, delete all breakpoints for this file
     */
    void DeleteSourceBreakpoint(const wxString& path, int lineNumber);

    /**
     * @brief delete function breakpoint from the store
     */
    void DeleteFunctionBreakpoint(const wxString& name);

    /**
     * @brief return all source breakpoints for a given file. If file is empty, return all the breakpoints
     */
    size_t GetAllSourceBreakpoints(const wxString& filepath, std::unordered_set<dap::SourceBreakpoint>* output) const;

    /**
     * @brief return all sources that have breakpoints in the store
     */
    size_t GetAllSources(std::vector<wxString>* sources) const;

    /**
     * @brief return all function breakpoints
     */
    size_t GetAllFunctionBreakpoints(std::unordered_set<dap::FunctionBreakpoint>* output) const;
};
} // namespace dap
#endif // CLDEBUGGERBREAKPOINTSTORE_HPP
