#ifndef LANGUAGESERVERPAGE_H
#define LANGUAGESERVERPAGE_H

#include "UI.h"
#include "LanguageServerEntry.h"

class LanguageServerPage : public LanguageServerPageBase
{
public:
    LanguageServerPage(wxWindow* parent, const LanguageServerEntry& data);
    virtual ~LanguageServerPage();
};
#endif // LANGUAGESERVERPAGE_H
