#ifndef CSCODECOMPLETEPHPHANDLER_H
#define CSCODECOMPLETEPHPHANDLER_H

#include "csCommandHandlerBase.h"

class csCodeCompletePhpHandler : public csCommandHandlerBase
{
    wxString m_path;
    wxString m_symbolsPath;
    int m_position;

public:
    virtual void DoProcessCommand(const JSONElement& options);

    csCodeCompletePhpHandler(wxEvtHandler* sink);
    virtual ~csCodeCompletePhpHandler();
};

#endif // CSCODECOMPLETEPHPHANDLER_H
