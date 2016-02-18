#ifndef CLEDITORWORDCHARSLOCKER_H
#define CLEDITORWORDCHARSLOCKER_H

#include "codelite_exports.h"
#include <wx/stc/stc.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clEditorWordCharsLocker
{
    wxStyledTextCtrl* m_stc;
    wxString m_origWordChars;

public:
    clEditorWordCharsLocker(wxStyledTextCtrl* stc, const wxString& newWordChars);
    ~clEditorWordCharsLocker();
};

#endif // CLEDITORWORDCHARSLOCKER_H
