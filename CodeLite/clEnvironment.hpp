#ifndef CLENVIRONMENT_HPP
#define CLENVIRONMENT_HPP

#include "codelite_exports.h"
#include "macros.h"
#include "wxStringHash.h"

#include <vector>
#include <wx/any.h>
#include <wx/string.h>

typedef std::vector<std::pair<wxString, wxString>> clEnvList_t;

class WXDLLIMPEXP_CL clEnvironment
{
    const clEnvList_t* m_env = nullptr;
    std::vector<std::pair<wxString, wxAny>> m_old_env;

private:
    void ApplyFromList(const clEnvList_t* envlist);

public:
    clEnvironment(const clEnvList_t* envlist);
    clEnvironment();
    virtual ~clEnvironment();
};

#endif // CLENVIRONMENT_HPP
