#ifndef CSPARSEPHPFOLDERHANDLER_H
#define CSPARSEPHPFOLDERHANDLER_H

#include "csCommandHandlerBase.h"
#include <wx/string.h>

class csParsePHPFolderHandler : public csCommandHandlerBase
{
    wxString m_folder;
    wxString m_mask;
    wxString m_dbpath;

protected:
    virtual void DoProcessCommand(const JSONItem& options);

public:
    csParsePHPFolderHandler(csManager* manager);
    virtual ~csParsePHPFolderHandler();
};

#endif // CSPARSEPHPFOLDERHANDLER_H
