#ifndef LANGUAGESERVERPAGE_H
#define LANGUAGESERVERPAGE_H

#include "LanguageServerEntry.h"
#include "UI.h"

class LanguageServerPage : public LanguageServerPageBase
{
protected:
    void InitialiseSSH(const LanguageServerEntry& data);

public:
    LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data);
    LanguageServerPage(wxWindow* parent);
    virtual ~LanguageServerPage();

    wxArrayString GetLanguages() const;
    LanguageServerEntry GetData() const;

protected:
    virtual void OnRemoteServerUI(wxUpdateUIEvent& event);
    virtual void OnBrowseWD(wxCommandEvent& event);
    virtual void OnCommandUI(wxUpdateUIEvent& event);
    virtual void OnSuggestLanguages(wxCommandEvent& event);
};
#endif // LANGUAGESERVERPAGE_H
