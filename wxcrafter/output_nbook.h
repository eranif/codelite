#ifndef SOURCEPANEL_H
#define SOURCEPANEL_H

#include "wx/notebook.h"
#include <wx/stc/stc.h>

class OutputNBook : public wxNotebook
{
public:
    OutputNBook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxNotebookNameStr);
    virtual ~OutputNBook();

    void CppPageSelected(wxStyledTextCtrl* cpptext, wxStyledTextCtrl* headertext) const;
    void XrcPageSelected(wxStyledTextCtrl* text) const;
};

#endif // SOURCEPANEL_H
