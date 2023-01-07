#ifndef DUPLICATETLWDLG_H
#define DUPLICATETLWDLG_H
#include "wxcrafter.h"

class DuplicateTLWDlg : public DuplicateTLWBaseDlg
{
public:
    DuplicateTLWDlg(wxWindow* parent);
    virtual ~DuplicateTLWDlg();

    wxString GetBaseName() const { return m_textBasename->GetValue(); }
    wxString GetInheritedName() const { return m_textInheritedName->GetValue(); }
    wxString GetFilename() const { return m_textFilename->GetValue(); }

protected:
    virtual void OnFilenameFocus(wxFocusEvent& event);
    virtual void OnInheritedNameFocus(wxFocusEvent& event);
};
#endif // DUPLICATETLWDLG_H
