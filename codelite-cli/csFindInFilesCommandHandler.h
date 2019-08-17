#ifndef CSFINDINFILESCOMMANDHANDLER_H
#define CSFINDINFILESCOMMANDHANDLER_H

#include "csCommandHandlerBase.h" // Base class: csCommandHandlerBase
#include <wx/string.h>

class csFindInFilesCommandHandler : public csCommandHandlerBase
{
    wxString m_folder;
    wxString m_what;
    wxString m_mask;
    bool m_case;
    bool m_word;

public:
    virtual void DoProcessCommand(const JSONItem& options);

public:
    csFindInFilesCommandHandler(csManager* manager);
    virtual ~csFindInFilesCommandHandler();
};

#endif // CSFINDINFILESCOMMANDHANDLER_H
