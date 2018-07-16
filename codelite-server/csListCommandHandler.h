#ifndef CSLISTCOMMANDHANDLER_H
#define CSLISTCOMMANDHANDLER_H

#include "csCommandHandlerBase.h"
class csListCommandHandler : public csCommandHandlerBase
{
public:
    csListCommandHandler(wxEvtHandler* sink);
    virtual ~csListCommandHandler();
    virtual void Process(const wxString& message);
};

#endif // CSLISTCOMMANDHANDLER_H
