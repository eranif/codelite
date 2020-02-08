#ifndef CLSSHAGENT_HPP
#define CLSSHAGENT_HPP

#include "codelite_exports.h"
#include <wx/sharedptr.h>

class WXDLLIMPEXP_CL clSSHAgent
{
private:
    int m_sshAgentPID = wxNOT_FOUND;

protected:
    void Start();
    void Stop();

public:
    typedef wxSharedPtr<clSSHAgent> Ptr_t;

public:
    clSSHAgent();
    virtual ~clSSHAgent();
};

#endif // CLSSHAGENT_HPP
