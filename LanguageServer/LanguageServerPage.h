#ifndef LANGUAGESERVERPAGE_H
#define LANGUAGESERVERPAGE_H

#include "LanguageServerEntry.h"
#include "UI.h"

class LanguageServerPage : public LanguageServerPageBase
{
public:
    LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data);
    LanguageServerPage(wxWindow* parent);
    virtual ~LanguageServerPage();

    wxArrayString GetLanguages() const;
    LanguageServerEntry GetData() const;

protected:
    void OnBrowseWD(wxCommandEvent& event) override;
    void OnCommandUI(wxUpdateUIEvent& event) override;
    void OnSuggestLanguages(wxCommandEvent& event) override;
};
#endif // LANGUAGESERVERPAGE_H
