#ifndef WXCNOTEBOOKCODEHELPER_H
#define WXCNOTEBOOKCODEHELPER_H

#include <wx/string.h>

/// A helper class that allows us to place
/// code after the pages creation has completed
/// An example is calling ExpandNode() after all
/// pages were added to the wxTreebook
class wxcNotebookCodeHelper
{
    wxString m_code;

private:
    wxcNotebookCodeHelper();
    virtual ~wxcNotebookCodeHelper();

public:
    static wxcNotebookCodeHelper& Get();

    void Clear();
    wxString& Code();
};

#endif // WXCNOTEBOOKCODEHELPER_H
