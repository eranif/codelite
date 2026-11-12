#pragma once

#include "codelite_exports.h"

#include <wx/stc/stc.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clSnippetManager
{
protected:
    int InsertTextSimple(wxStyledTextCtrl* ctrl, const wxString& text);
    void SetCaretAt(wxStyledTextCtrl* ctrl, int pos);

public:
    clSnippetManager() = default;
    virtual ~clSnippetManager() = default;

    static clSnippetManager& Get();
    /**
     * @brief insert a code snippet into the editor
     */
    void Insert(wxStyledTextCtrl* ctrl, const wxString& snippet);
};

