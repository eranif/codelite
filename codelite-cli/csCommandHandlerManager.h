#ifndef CSCOMMANDHANDLERMANAGER_H
#define CSCOMMANDHANDLERMANAGER_H

#include "csCommandHandlerBase.h"
#include <wxStringHash.h>

class csCommandHandlerManager
{
    std::unordered_map<wxString, csCommandHandlerBase::Ptr_t> m_handlers;

public:
    csCommandHandlerManager();
    virtual ~csCommandHandlerManager();

    void Register(const wxString& command, csCommandHandlerBase::Ptr_t handler);
    csCommandHandlerBase::Ptr_t FindHandler(const wxString& command) const;
};

#endif // CSCOMMANDHANDLERMANAGER_H
