#ifndef CLEDITORWORDCHARSLOCKER_H
#define CLEDITORWORDCHARSLOCKER_H

#include "codelite_exports.h"
#include <wx/stc/stc.h>
#include <wx/string.h>
#include <set>

class WXDLLIMPEXP_SDK clEditorWordCharsLocker
{
    wxStyledTextCtrl* m_stc;
    wxString m_wordChars;
    std::set<int> m_chars;
    
public:
    clEditorWordCharsLocker(wxStyledTextCtrl* stc, const wxString& wordChars);
    ~clEditorWordCharsLocker();
    
    /**
     * @brief return the word from given pos
     * This function uses the 'm_wordChars' to decided on teh word boundaries
     */
    wxString GetWordAtPos(int pos, int &startPos, int &endPos) const;
};

#endif // CLEDITORWORDCHARSLOCKER_H
