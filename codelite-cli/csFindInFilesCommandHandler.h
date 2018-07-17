#ifndef CSFINDINFILESCOMMANDHANDLER_H
#define CSFINDINFILESCOMMANDHANDLER_H

#include "csCommandHandlerBase.h" // Base class: csCommandHandlerBase

class csFindInFilesCommandHandler : public csCommandHandlerBase
{
public:
    csFindInFilesCommandHandler(wxEvtHandler* sink);
    virtual ~csFindInFilesCommandHandler();

public:
    virtual void Process(const JSONElement& options);
};

#endif // CSFINDINFILESCOMMANDHANDLER_H
