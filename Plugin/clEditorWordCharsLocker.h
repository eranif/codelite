#ifndef CLEDITORWORDCHARSLOCKER_H
#define CLEDITORWORDCHARSLOCKER_H

#include "codelite_exports.h"

#include <unordered_set>
#include <wx/stc/stc.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clEditorXmlHelper
{
    wxStyledTextCtrl* m_stc;
    wxString m_wordChars;
    std::unordered_set<int> m_chars;

protected:
    int GetCharAt(int pos) const;

public:
    clEditorXmlHelper(wxStyledTextCtrl* stc);
    ~clEditorXmlHelper() = default;

    /**
     * @brief return the word from given pos
     * This function uses the 'm_wordChars' to decide on the word boundaries
     */
    wxString GetXmlTagAt(int pos, int& startPos, int& endPos) const;
};

#endif // CLEDITORWORDCHARSLOCKER_H
