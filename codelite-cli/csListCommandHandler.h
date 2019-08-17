#ifndef CSLISTCOMMANDHANDLER_H
#define CSLISTCOMMANDHANDLER_H

#include "csCommandHandlerBase.h"
#include <wx/string.h>

class csListCommandHandler : public csCommandHandlerBase
{
    wxString m_folder;

public:
    virtual void DoProcessCommand(const JSONItem& options);

public:
    csListCommandHandler(csManager* manager);
    virtual ~csListCommandHandler();
};

#endif // CSLISTCOMMANDHANDLER_H
