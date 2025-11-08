#ifndef CLSSHAGENT_HPP
#define CLSSHAGENT_HPP

#include "codelite_exports.h"

#include <memory>
#include <wx/arrstr.h>

class IProcess;
class WXDLLIMPEXP_CL clSSHAgent
{
private:
    IProcess* m_process = nullptr;
    wxArrayString m_files;

protected:
    void Start();
    void Stop();

public:
    using Ptr_t = std::shared_ptr<clSSHAgent>;

public:
    clSSHAgent(const wxArrayString& files = {});
    virtual ~clSSHAgent();
};

#endif // CLSSHAGENT_HPP
