#ifndef CSPARSEFOLDERHANDLER_H
#define CSPARSEFOLDERHANDLER_H

#include "csCommandHandlerBase.h"
#include "csCommandHandlerManager.h"
#include <wx/string.h>

class csParseFolderHandler : public csCommandHandlerBase
{
    wxString m_language;
    wxString m_path;
    csCommandHandlerManager m_parseHandlers;

public:
    virtual void DoProcessCommand(const JSONItem& options);

public:
    csParseFolderHandler(csManager* manager);
    virtual ~csParseFolderHandler();
};

#endif // CSPARSEFOLDERHANDLER_H
