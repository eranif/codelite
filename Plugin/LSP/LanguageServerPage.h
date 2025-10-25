#ifndef LANGUAGESERVERPAGE_H
#define LANGUAGESERVERPAGE_H

#include "LanguageServerEntry.h"
#include "UI.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LanguageServerPage : public LanguageServerPageBase
{
public:
    LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data);
    LanguageServerPage(wxWindow* parent);
    virtual ~LanguageServerPage() = default;

    wxArrayString GetLanguages() const;
    LanguageServerEntry GetData() const;
    bool ValidateData(wxString* message) const;

protected:
    void OnBrowseWD(wxCommandEvent& event) override;
    void OnCommandUI(wxUpdateUIEvent& event) override;
    void OnSuggestLanguages(wxCommandEvent& event) override;
};
#endif // LANGUAGESERVERPAGE_H
