#ifndef CSCODECOMPLETEPHPHANDLER_H
#define CSCODECOMPLETEPHPHANDLER_H

#include "csCommandHandlerBase.h"

class csCodeCompletePhpHandler : public csCommandHandlerBase
{
    wxString m_path;
    wxString m_unsavedBufferPath;
    wxString m_symbolsPath;
    int m_position;

public:
    virtual void DoProcessCommand(const JSONItem& options);

    csCodeCompletePhpHandler(csManager* manager);
    virtual ~csCodeCompletePhpHandler();
};

#endif // CSCODECOMPLETEPHPHANDLER_H
