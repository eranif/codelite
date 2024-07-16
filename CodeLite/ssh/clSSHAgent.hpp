#ifndef CLSSHAGENT_HPP
#define CLSSHAGENT_HPP

#include "codelite_exports.h"

#include <memory>
#include <wx/arrstr.h>
#include <wx/sharedptr.h>

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
    typedef std::shared_ptr<clSSHAgent> Ptr_t;

public:
    clSSHAgent(const wxArrayString& files = {});
    virtual ~clSSHAgent();
};

#endif // CLSSHAGENT_HPP
