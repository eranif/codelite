#ifndef BREAKPOINTSMAP_HPP
#define BREAKPOINTSMAP_HPP

#include "dap/dap.hpp"

class SessionBreakpoints
{
    std::vector<dap::Breakpoint> m_breakpoints;

private:
    int find_by_id_internal(int id);
    size_t find_by_path_internal(const wxString& path, std::vector<dap::Breakpoint>* bps);

public:
    SessionBreakpoints() = default;
    ~SessionBreakpoints() = default;

    /**
     * @brief clear all the stored breakpoints
     */
    void clear();

    /**
     * @brief find breakpoint by ID
     */
    bool find_by_id(int id, dap::Breakpoint* bp);

    /**
     * @brief update or insert. this is done by the id property of bp
     */
    bool update_or_insert(const dap::Breakpoint& bp);

    // getters
    const std::vector<dap::Breakpoint>& get_breakpoints() const { return m_breakpoints; }

    /**
     * @brief delete breakpoint by ID
     */
    void delete_by_id(int id);

    /**
     * @brief delete all breakpoints belonged to path
     */
    void delete_by_path(const wxString& path);
    /**
     * @brief given list of breakpoints, delete all the breakpoints with similar paths
     */
    void delete_by_paths(const std::vector<dap::Breakpoint>& bps);
};

#endif // BREAKPOINTSMAP_HPP
