#ifndef CLSTATUSBARMESSAGE_H
#define CLSTATUSBARMESSAGE_H

#include "codelite_exports.h"

#include <wx/string.h>

class WXDLLIMPEXP_SDK clStatusBarMessage
{
    wxString m_startMessage;
    wxString m_endMessage;

public:
    clStatusBarMessage(const wxString& startMessage, const wxString& endMessage = wxEmptyString);
    virtual ~clStatusBarMessage();
};

#endif // CLSTATUSBARMESSAGE_H
