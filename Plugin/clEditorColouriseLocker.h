#ifndef CLEDITORCOLOURISELOCKER_H
#define CLEDITORCOLOURISELOCKER_H

#include "codelite_exports.h"

#include <wx/stc/stc.h>

class WXDLLIMPEXP_SDK clEditorColouriseLocker
{
    wxStyledTextCtrl* m_stc;
    int m_firstPos;
    int m_lastPos;

public:
    clEditorColouriseLocker(wxStyledTextCtrl* stc);
    ~clEditorColouriseLocker();
};

#endif // CLEDITORCOLOURISELOCKER_H
