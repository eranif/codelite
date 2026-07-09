#ifndef CLENVIRONMENT_HPP
#define CLENVIRONMENT_HPP

#include "codelite_exports.h"

#include <optional>
#include <vector>
#include <wx/string.h>

using clEnvList_t = std::vector<std::pair<wxString, wxString>>;

class WXDLLIMPEXP_CL clEnvironment
{
public:
    clEnvironment(const clEnvList_t* envlist);
    clEnvironment();
    virtual ~clEnvironment();

    void ApplyFromList(const clEnvList_t* envlist);

private:
    const clEnvList_t* m_env = nullptr;
    std::vector<std::pair<wxString, std::optional<wxString>>> m_old_env;
};

/**
 * @brief given environment string in form of: `a=b;c=d` construct a clEnvList_t
 */
WXDLLIMPEXP_CL clEnvList_t BuildEnvFromString(const wxString& env_str);

/**
 * @brief using the current environment variables, resolve the list
 */
WXDLLIMPEXP_CL clEnvList_t ResolveEnvList(const clEnvList_t& env_list);
WXDLLIMPEXP_CL clEnvList_t ResolveEnvList(const wxString& envstr);

#endif // CLENVIRONMENT_HPP
