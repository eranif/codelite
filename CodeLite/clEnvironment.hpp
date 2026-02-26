#ifndef CLENVIRONMENT_HPP
#define CLENVIRONMENT_HPP

#include "codelite_exports.h"

#include <vector>
#include <wx/any.h>
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
    std::vector<std::pair<wxString, wxAny>> m_old_env;
};

#endif // CLENVIRONMENT_HPP
