#ifndef LANGUAGESERVERPAGE_H
#define LANGUAGESERVERPAGE_H

#include "UI.h"
#include "LanguageServerEntry.h"

class LanguageServerPage : public LanguageServerPageBase
{
public:
    LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data);
    LanguageServerPage(wxWindow* parent);
    virtual ~LanguageServerPage();
    
    wxArrayString GetLanguages() const;
    LanguageServerEntry GetData() const;
};
#endif // LANGUAGESERVERPAGE_H
