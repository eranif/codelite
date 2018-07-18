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
    virtual void DoProcessCommand(const JSONElement& options);

public:
    csParsePHPFolderHandler(wxEvtHandler* sink);
    virtual ~csParsePHPFolderHandler();
};

#endif // CSPARSEPHPFOLDERHANDLER_H
