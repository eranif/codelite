#ifndef CSLISTCOMMANDHANDLER_H
#define CSLISTCOMMANDHANDLER_H

#include "csCommandHandlerBase.h"
#include <wx/string.h>

class csListCommandHandler : public csCommandHandlerBase
{
    wxString m_folder;

public:
    virtual void DoProcessCommand(const JSONElement& options);

public:
    csListCommandHandler(wxEvtHandler* sink);
    virtual ~csListCommandHandler();
};

#endif // CSLISTCOMMANDHANDLER_H
