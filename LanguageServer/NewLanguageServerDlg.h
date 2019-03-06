#ifndef NEWLANGUAGESERVERDLG_H
#define NEWLANGUAGESERVERDLG_H

#include "UI.h"
#include "LanguageServerEntry.h"
#include <wx/arrstr.h>

class NewLanguageServerDlg : public NewLanguageServerDlgBase
{
public:
    NewLanguageServerDlg(wxWindow* parent);
    virtual ~NewLanguageServerDlg();

    LanguageServerEntry GetData() const;

protected:
    virtual void OnOKUI(wxUpdateUIEvent& event);
    wxArrayString GetLanguages() const;
};
#endif // NEWLANGUAGESERVERDLG_H
