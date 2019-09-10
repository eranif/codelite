#ifndef CSCODECOMPLETEHANDLER_H
#define CSCODECOMPLETEHANDLER_H

#include "csCommandHandlerBase.h"
#include "csCommandHandlerManager.h"
#include <wx/string.h>

class csCodeCompleteHandler : public csCommandHandlerBase
{
    csCommandHandlerManager m_codeCompleteHandlers;
    wxString m_lang;

public:
    virtual void DoProcessCommand(const JSONItem& options);
    csCodeCompleteHandler(csManager* manager);
    virtual ~csCodeCompleteHandler();
};

#endif // CSCODECOMPLETEHANDLER_H
